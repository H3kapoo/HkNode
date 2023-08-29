#include "HkConstraintContext.hpp"

#include "../base/HkNodeBase.hpp"
#include "../utils/HkDrawDebugger.hpp"

namespace hkui
{

/* Set TC this ctx will be based on*/
void HkConstraintContext::setRootTc(HkTransformContext* rootTc) { thisTc_ = rootTc; }

/* Set policy of TC children */
void HkConstraintContext::setPolicy(const HkConstraintPolicy policy) { policy_ = policy; }

void HkConstraintContext::resolveConstraints(std::vector<HkTreeStructure<HkNodeBase>*>& children)
{
    if (children.empty())
        return;

    /* TODO: dirty flags shall be used here to not do redundant repositioning */
    switch (policy_)
    {
    case HkConstraintPolicy::AlignLeftToRight:
        alignLeftRight(children);
        break;
    case HkConstraintPolicy::AlignTopToBottom:
        alignTopBottom(children);
        break;
    case HkConstraintPolicy::AlignEvenLeftToRight:
        alignEvenLeftRight(children);
        break;
    case HkConstraintPolicy::AlignCenterLeftToRight:
        alignCenterLeftRight(children);
        break;
    }
}

void HkConstraintContext::computeScrollBarCount()
{
    sbCount_ = 0;
    sbCount_ += (isOverflowAllowedX_ && overflowXYSize_.x) ? 1 : 0;
    sbCount_ += (isOverflowAllowedY_ && overflowXYSize_.y) ? 1 : 0;
}

void HkConstraintContext::computeOverflowBasedOnMinMax(const MinMaxPos& minMax)
{
    isOverflowX_ = false;
    overflowXYSize_.x = 0;
    if (minMax.max > thisTc_->getScale().x)
    {
        isOverflowX_ = true;
        overflowXYSize_.x = minMax.max - thisTc_->getScale().x;
    }

    if (minMax.min < thisTc_->getPos().x)
    {
        isOverflowX_ = true;
        overflowXYSize_.x += thisTc_->getPos().x - minMax.min;
    }
}

MinMaxPos HkConstraintContext::getMinAndMaxPositions(const std::vector<HkTreeStructure<HkNodeBase>*>& children)
{
    MinMaxPos result;
    result.min = 99999;//99999; // should be int32 max but this is good enough
    result.max = -99999;
    for (uint32_t i = 0; i < children.size() - sbCount_; i++)
    {
        const auto& childTc = children[i]->getPayload()->node_.transformContext;
        if (childTc.getPos().x <= result.min)
        {
            result.min = childTc.getPos().x;
        }

        if (childTc.getPos().x + childTc.getScale().x >= result.max)
        {
            result.max = childTc.getPos().x + childTc.getScale().x;
        }
    }
    return result;
}

ScrollbarMargin HkConstraintContext::getScrollbarMargins(const std::vector<HkTreeStructure<HkNodeBase>*>& children) const
{
    //Note: we can deduce which SB is which by looking at its TC:
    // VSBs have their height > width, vice versa for HSBs. 99% of the cases
    //Note: this works as long as sbs have same margin, which will be the case
    ScrollbarMargin margins;
    for (const auto& child : children)
    {
        if (child->getType() != "ScrollBar") continue;
        const auto scale = child->getPayload()->node_.transformContext.getScale();
        if (scale.x > scale.y)
        {
            margins.hsbMargin = scale.y;
        }
        else
        {
            margins.vsbMargin = scale.x;
        }
    }
    return margins;
}

/* Gets the max length of a children plus the total vertical length of the children. Excludes scrollbar children*/
MaxAndTotal HkConstraintContext::getVerticalMaxValueAndTotalHeightValue(
    const std::vector<HkTreeStructure<HkNodeBase>*>& children)
{
    MaxAndTotal mt;
    mt.total = std::accumulate(children.begin(), children.end(), 0,
        [&mt](int total, HkTreeStructure<HkNodeBase>* child)
        {
            /* Skip ScrollBars from width calc */
            //TODO: Maybe introduce enum types so we dont compare strings each time?
            if (child->getType() == "ScrollBar") return total;

            /* Compute max value seen so far */
            const auto scaleY = child->getPayload()->node_.transformContext.getScale().y;
            if (scaleY > mt.max) mt.max = scaleY;

            return total + scaleY;
        });
    return mt;
}

/* Gets the max length of a children plus the total horizontal length of the children. Excludes scrollbar children*/
MaxAndTotal HkConstraintContext::getHorizontalMaxValueAndTotalWidthValue(
    const std::vector<HkTreeStructure<HkNodeBase>*>& children)
{
    MaxAndTotal mt;
    mt.total = std::accumulate(children.begin(), children.end(), 0,
        [&mt](int total, HkTreeStructure<HkNodeBase>* child)
        {
            /* Skip ScrollBars from width calc */
            //TODO: Maybe introduce enum types so we dont compare strings each time?
            if (child->getType() == "ScrollBar") return total;

            /* Compute max value seen so far */
            const auto scaleX = child->getPayload()->node_.transformContext.getScale().x;
            if (scaleX > mt.max) mt.max = scaleX;

            return total + scaleX;
        });

    return mt;
}

//TODO: maybe they can be combined into a single pair
/* Computes X Y overflow truth value plus by how much they overflow */
void HkConstraintContext::resolveChildrenOverflowVariables(const HkChildrenOrientation orientation,
    const std::vector<HkTreeStructure<HkNodeBase>*>& children)
{
    const auto scrollBarMargins = getScrollbarMargins(children);
    const auto maxAndTotalVert = getVerticalMaxValueAndTotalHeightValue(children);
    const auto maxAndTotalHori = getHorizontalMaxValueAndTotalWidthValue(children);

    if (orientation == HkChildrenOrientation::Vertical)
    {
        /* Compute both possible Y overflow situations */
        /* Overflow in this context can only occur if one children is too big Y wise vs Parent OR if
           combined children heights are bigger than Parent's.*/
        overflowXYSize_.y = std::max(maxAndTotalVert.max - thisTc_->getScale().y, maxAndTotalVert.total - thisTc_->getScale().y);
        /*Account for scrollbar height */
        overflowXYSize_.y += isOverflowX_ ? scrollBarMargins.hsbMargin : 0;
        if (overflowXYSize_.y > 0)
        {
            isOverflowY_ = true;
        }
        else
        {
            /* If overflow is not occuring, we do not care about overflow value, so reset to 0 */
            overflowXYSize_.y = 0;
            isOverflowY_ = false;
        }

        /* Compute the single possible X overflow situation */
        /* Overflow in this context can only occur if one children is too big X wise vs Parent */
        overflowXYSize_.x = thisTc_->getPos().x + maxAndTotalHori.max + (isOverflowY_ ? scrollBarMargins.vsbMargin : 0) - (thisTc_->getPos().x + thisTc_->getScale().x);
        if (overflowXYSize_.x > 0)
        {
            isOverflowX_ = true;
        }
        else
        {
            overflowXYSize_.x = 0;
            isOverflowX_ = false;
        }
    }
    else if (orientation == HkChildrenOrientation::Horizontal)
    {
        /* Compute both possible X overflow situations */
        /* Overflow in this context can only occur if one children is too big X wise vs Parent OR if
            combined children widths are bigger than Parent's.*/
        overflowXYSize_.x = std::max(maxAndTotalHori.max - thisTc_->getScale().x, maxAndTotalHori.total - thisTc_->getScale().x);
        /*Account for scrollbar height */
        overflowXYSize_.x += isOverflowY_ ? scrollBarMargins.vsbMargin : 0;
        if (overflowXYSize_.x > 0)
        {
            isOverflowX_ = true;
        }
        else
        {
            /* If overflow is not occuring, we do not care about overflow value, so reset to 0 */
            overflowXYSize_.x = 0;
            isOverflowX_ = false;
        }

        /* Compute the single possible Y overflow situation */
        /* Overflow in this context can only occur if one children is too big Y wise vs Parent */
        overflowXYSize_.y = thisTc_->getPos().y + maxAndTotalVert.max + (isOverflowX_ ? scrollBarMargins.hsbMargin : 0) - (thisTc_->getPos().y + thisTc_->getScale().y);
        if (overflowXYSize_.y > 0)
        {
            isOverflowY_ = true;
        }
        else
        {
            overflowXYSize_.y = 0;
            isOverflowY_ = false;
        }
    }
}

//TODO: This shall be renamed something like "Layouting", horizontal or vertical so then we can
//      use methods to center them or left/right align them
/*
    Positions children one on top of the other, left aligned
*/
void HkConstraintContext::alignTopBottom(const std::vector<HkTreeStructure<HkNodeBase>*>& children)
{
    resolveChildrenOverflowVariables(HkChildrenOrientation::Vertical, children);

    auto startPosX = thisTc_->getPos().x + offsetPercentage_.x * -overflowXYSize_.x;
    auto startPosY = thisTc_->getPos().y + offsetPercentage_.y * -overflowXYSize_.y;
    for (const auto& child : children)
    {
        if (child->getType() == "ScrollBar") continue;

        auto& childTc = child->getPayload()->node_.transformContext;
        childTc.setPos({ startPosX, startPosY });
        startPosY += childTc.getScale().y;
    }
}

void HkConstraintContext::alignCenterLeftRight(const std::vector<HkTreeStructure<HkNodeBase>*>& children)
{
    resolveChildrenOverflowVariables(HkChildrenOrientation::Horizontal, children);

    const auto maxAndTotalHori = getHorizontalMaxValueAndTotalWidthValue(children);
    // const auto slotSize = (thisTc_->getScale().x + overflowXYSize_.x) / children.size();
    // const auto slotSize = (thisTc_->getScale().x) / children.size();
    const auto center = thisTc_->getScale().x / 2;
    const auto leftStart = center - maxAndTotalHori.total / 2;
    const auto rightStart = center + maxAndTotalHori.total / 2;

    auto startPosX = leftStart + offsetPercentage_.x * -overflowXYSize_.x;
    auto startPosY = thisTc_->getPos().y + offsetPercentage_.y * -overflowXYSize_.y;

    //TODO: overflow due to repositioning of children does not occur
    //TODO: need new method to calculate if theres an overflow. Position should be taken into account as well
    HkDrawDebugger::get().pushDraw10x10({
        center,
        300 });
    HkDrawDebugger::get().pushDraw10x10({
        leftStart,
        300 });
    HkDrawDebugger::get().pushDraw10x10({
        rightStart,
        300 });

    for (const auto& child : children)
    {
        if (child->getType() == "ScrollBar") continue;

        auto& childTc = child->getPayload()->node_.transformContext;
        childTc.setPos({ startPosX , startPosY });
        startPosX += childTc.getScale().x;
    }
}

void HkConstraintContext::alignEvenLeftRight(const std::vector<HkTreeStructure<HkNodeBase>*>& children)
{
    computeScrollBarCount();

    const auto slotSize = thisTc_->getScale().x / (children.size() - sbCount_);
    auto startPosX = thisTc_->getPos().x + slotSize / 2;
    auto startPosY = thisTc_->getPos().y;

    for (uint32_t i = 0; i < children.size() - sbCount_; i++)
    {
        auto& childTc = children[i]->getPayload()->node_.transformContext;
        childTc.setPos({
            startPosX + slotSize * i - childTc.getScale().x / 2,
            startPosY });
    }

    /* Compute scrollbars count first as this can influence the result */
    computeScrollBarCount();

    /* Compute min max values after initial children reposition , this also*/
    MinMaxPos result = getMinAndMaxPositions(children);

    /* Compute overflow variables */
    computeOverflowBasedOnMinMax(result);

    /* Account for scrollbar scrolling and left edge overflow */
    for (uint32_t i = 0; i < children.size() - sbCount_; i++)
    {
        auto& childTc = children[i]->getPayload()->node_.transformContext;
        const auto leftOverflow = result.min < 0 ? -result.min : 0;
        childTc.addPos({ leftOverflow + offsetPercentage_.x * -overflowXYSize_.x, childTc.getPos().y });
    }
}

/*
    Place children from left to right, simple as that
*/
void HkConstraintContext::alignLeftRight(const std::vector<HkTreeStructure<HkNodeBase>*>& children)
{
    resolveChildrenOverflowVariables(HkChildrenOrientation::Horizontal, children);

    auto startPosX = thisTc_->getPos().x + offsetPercentage_.x * -overflowXYSize_.x;
    auto startPosY = thisTc_->getPos().y + offsetPercentage_.y * -overflowXYSize_.y;
    for (const auto& child : children)
    {
        if (child->getType() == "ScrollBar") continue;

        auto& childTc = child->getPayload()->node_.transformContext;
        childTc.setPos({ startPosX , startPosY });
        startPosX += childTc.getScale().x;
    }
}

/* This simply constraints the Knob inside the scrollbar itself, at given currentValue (0 to 1) */
void HkConstraintContext::constrainSBKnob(bool isFromHorizontalSB, int overflowSize, float currKnobValue, HkTransformContext& knobTc)
{
    //setting scale of knob
    const auto preKNobSize = std::min(thisTc_->getScale().x, thisTc_->getScale().y);
    knobTc.setScale({ preKNobSize, preKNobSize });

    const auto margins = 2;
    const auto minAxisKobSize = 30;
    /* Orientation calcs do differ */
    if (isFromHorizontalSB)
    {
        const auto knobSizeX = thisTc_->getScale().x - overflowSize / 1;
        knobTc.setScale({ std::max(minAxisKobSize, knobSizeX) - 2 * margins , preKNobSize - 2 * margins });
        const auto minX = thisTc_->getPos().x + margins;
        const auto maxX = thisTc_->getPos().x + thisTc_->getScale().x - knobTc.getScale().x - margins;
        const auto posX = minX * (1.0f - currKnobValue) + currKnobValue * maxX;
        knobTc.setPos({ posX, thisTc_->getPos().y + margins });
    }
    else
    {
        const auto knobSizeY = thisTc_->getScale().y - overflowSize;
        knobTc.setScale({ preKNobSize - 2 * margins, std::max(minAxisKobSize, knobSizeY) - 2 * margins });

        const auto minY = thisTc_->getPos().y + margins;
        const auto maxY = thisTc_->getPos().y + thisTc_->getScale().y - knobTc.getScale().y - margins;
        const auto posY = minY * (1.0f - currKnobValue) + currKnobValue * maxY;
        knobTc.setPos({ thisTc_->getPos().x + margins, posY });
    }
}

/* Keeps scrollbar object at the bottom or at the right of the container. */
void HkConstraintContext::scrollBarConstrain(HkTransformContext& scrollBarTc)
{
    ScrollbarMargin margins;
    const auto sbScale = scrollBarTc.getScale();
    (sbScale.x > sbScale.y) ? (margins.hsbMargin = sbScale.y) : (margins.vsbMargin = sbScale.x);

    const auto barScale = std::max(margins.hsbMargin, margins.vsbMargin);
    if (margins.hsbMargin && isOverflowX_)
    {
        /* We should take into account if vertical bar is present so that bottom right 'intersection'
           between bars is filled or not. Same thing shoukd apply for vertical calcs bellow */
        const auto verticalBarAwareMargin = isOverflowY_ ? barScale : 0;
        scrollBarTc.setScale({ thisTc_->getScale().x - verticalBarAwareMargin, barScale });
        scrollBarTc.setPos({ thisTc_->getPos().x, thisTc_->getPos().y + thisTc_->getScale().y - barScale });
    }
    else if (margins.vsbMargin && isOverflowY_)
    {
        const auto horizontalBarAwareMargin = isOverflowX_ ? barScale : 0;
        scrollBarTc.setScale({ barScale, thisTc_->getScale().y - horizontalBarAwareMargin });
        scrollBarTc.setPos({ thisTc_->getPos().x + thisTc_->getScale().x - barScale, thisTc_->getPos().y });
    }
}

/* WindowFrame is a special UI element that 'drags' a container along with it that sits underneath the window frame. This
   function helps constraint that container to the windowFrame element */
void HkConstraintContext::windowFrameContainerConstraint(HkTransformContext& wfCtr,
    HkTransformContext& exitBtn, HkTransformContext& minBtn) const
{
    /* TODO: dirty flags shall be used here to not do redundant repositioning */
    wfCtr.setPos({ thisTc_->getPos().x, thisTc_->getPos().y + thisTc_->getScale().y });
    wfCtr.setScale({ thisTc_->getScale().x, wfCtr.getScale().y });

    exitBtn.setScale({ 20, 20 }); // hardcoded, but technically ok situation
    exitBtn.setPos({
        thisTc_->getPos().x + thisTc_->getScale().x - 20 - 5,
        thisTc_->getPos().y + 20 / 2 - 5
        });

    minBtn.setScale({ 20, 20 });
    minBtn.setPos({
        thisTc_->getPos().x + thisTc_->getScale().x - 20 - 35,
        thisTc_->getPos().y + 20 / 2 - 5
        });
}

void HkConstraintContext::windowFrameContainerConstraint(HkTransformContext& wfCtr, HkTransformContext& exitBtn,
    HkTransformContext& minBtn, const glm::ivec2& windowSize, const bool isFullscreen) const
{
    if (!isFullscreen)
    {
        windowFrameContainerConstraint(wfCtr, exitBtn, minBtn);
        return;
    }

    thisTc_->setPos({ 0,0 });
    thisTc_->setScale({ 0,0 });

    wfCtr.setScale(windowSize + 1); //TODO: hacK: looks like wfCtr gets a windowSize thats lacking exactly 1px behind
    wfCtr.setPos({ 0,0 });
}
} // hkui