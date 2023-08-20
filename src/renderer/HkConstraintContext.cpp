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
    }
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
        if (overflowXYSize_.y >= 0)
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
        if (overflowXYSize_.x >= 0)
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
        if (overflowXYSize_.x >= 0)
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
        if (overflowXYSize_.y >= 0)
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

/*
    Place children from left to right, simple as that
*/
void HkConstraintContext::alignLeftRight(const std::vector<HkTreeStructure<HkNodeBase>*>& children)
{
    resolveChildrenOverflowVariables(HkChildrenOrientation::Horizontal, children);

    auto startPosX = thisTc_->getPos().x + offsetPercentage_.x * -(overflowXYSize_.x);
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
        const auto knobSizeX = thisTc_->getScale().x - overflowSize;
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
void HkConstraintContext::windowFrameContainerConstraint(HkTransformContext& childTc)
{
    /* TODO: dirty flags shall be used here to not do redundant repositioning */
    childTc.setPos({ thisTc_->getPos().x, thisTc_->getPos().y + thisTc_->getScale().y });
    childTc.setScale({ thisTc_->getScale().x, childTc.getScale().y });
}
} // hkui