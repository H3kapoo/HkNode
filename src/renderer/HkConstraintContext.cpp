#include "HkConstraintContext.hpp"

#include "../base/HkNodeBase.hpp"

namespace hkui
{

void HkConstraintContext::setRootTc(HkTransformContext* rootTc) { thisTc_ = rootTc; }

void HkConstraintContext::setPolicy(const HkConstraintPolicy policy) { policy_ = policy; }

void HkConstraintContext::resolveConstraints(std::vector<HkTreeStructure<HkNodeBase>*>& children)
{
    if (children.empty()) return;

    /* TODO: dirty flags shall be used here to not do redundant repositioning */

    switch (policy_)
    {
    case HkConstraintPolicy::AlignHorizontally:
    case HkConstraintPolicy::AlignLeftToRight:
    case HkConstraintPolicy::AlignTopToBottom:
        // alignHorizontally(children);
        // alignLeftRight(children);
        alignTopBottom(children);
        break;
    }
}

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
            const auto scaleY = child->getPayload()->node_.transformContext.scale.y;
            if (scaleY > mt.max) mt.max = scaleY;

            return total + scaleY;
        });
    return mt;
}

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
            const auto scaleX = child->getPayload()->node_.transformContext.scale.x;
            if (scaleX > mt.max) mt.max = scaleX;

            return total + scaleX;
        });
    return mt;
}

//TODO: BUG: Sometimes scroll knob movement causes weird int overflow. THis is occasional..maybe something's not inited
void HkConstraintContext::resolveChildrenOverflowVariables(const HkChildrenOrientation orientation,
    const std::vector<HkTreeStructure<HkNodeBase>*>& children)
{
    const auto maxAndTotalVert = getVerticalMaxValueAndTotalHeightValue(children);
    const auto maxAndTotalHori = getHorizontalMaxValueAndTotalWidthValue(children);

    if (orientation == HkChildrenOrientation::Vertical)
    {
        /* Compute both possible Y overflow situations */
        /* Overflow in this context can only occur if one children is too big Y wise vs Parent OR if
           combined children heights are bigger than Parent's.*/
        overflowXYSize_.y = std::max(maxAndTotalVert.max - thisTc_->scale.y, maxAndTotalVert.total - thisTc_->scale.y);
        if (overflowXYSize_.y >= 0)
        {
            isOverflowY_ = true;
            /*Account for scrollbar height */
            //TODO: hardcoded for now..
            overflowXYSize_.y += isOverflowX_ ? 20 : 0;
            // overflowXYSize_.x += 20;
        }
        else
        {
            /* If overflow is not occuring, we do not care about overflow value, so reset to 0 */
            overflowXYSize_.y = 0;
            isOverflowY_ = false;
        }

        /* Compute the single possible X overflow situation */
        /* Overflow in this context can only occur if one children is too big X wise vs Parent */
        overflowXYSize_.x = maxAndTotalHori.max - thisTc_->scale.x;
        if (overflowXYSize_.x >= 0)
        {
            isOverflowX_ = true;
            /*Account for scrollbar height */
            //TODO: hardcoded for now..
            overflowXYSize_.x += isOverflowY_ ? 20 : 0;
            // overflowXYSize_.y += 20;
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
        overflowXYSize_.x = std::max(maxAndTotalVert.max - thisTc_->scale.x, maxAndTotalVert.total - thisTc_->scale.x);
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
        overflowXYSize_.y = maxAndTotalHori.max - thisTc_->scale.y;
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

void HkConstraintContext::alignTopBottom(const std::vector<HkTreeStructure<HkNodeBase>*>& children)
{
    /*
        Centers children vertically inside the parent and places children one after the other vertically.
        Parent's X value is used for the children X value.
    */
    resolveChildrenOverflowVariables(HkChildrenOrientation::Vertical, children);

    auto startPosY = thisTc_->pos.y - thisTc_->scale.y / 2 + offsetPercentage_.y * -overflowXYSize_.y;
    auto startPosX = thisTc_->pos.x - thisTc_->scale.x / 2 + offsetPercentage_.x * -overflowXYSize_.x;
    for (const auto& child : children)
    {
        if (child->getType() == "ScrollBar") continue;

        auto& childTc = child->getPayload()->node_.transformContext;
        startPosY += childTc.scale.y / 2;
        childTc.setPos({ startPosX + childTc.scale.x / 2, startPosY });
        startPosY += childTc.scale.y / 2;
    }
}

void HkConstraintContext::alignLeftRight(const std::vector<HkTreeStructure<HkNodeBase>*>& children)
{
    /*
        Place children from left to right, simple as that
    */

    auto startPosX = thisTc_->pos.x - thisTc_->scale.x / 2 + offsetPercentage_.x * overflowXYSize_.x;
    for (const auto& child : children)
    {
        if (child->getType() == "ScrollBar") continue;

        auto& childTc = child->getPayload()->node_.transformContext;
        startPosX += childTc.scale.x / 2;
        childTc.setPos({ startPosX , thisTc_->pos.y });
        startPosX += childTc.scale.x / 2;
    }
}

void HkConstraintContext::alignVertically(const std::vector<HkTreeStructure<HkNodeBase>*>& children)
{
    /*
        Centers children vertically inside the parent and places children one after the other vertically.
        Parent's X value is used for the children X value.
    */
    const auto childrenHeight = std::accumulate(children.begin(), children.end(), 0,
        [](int total, HkTreeStructure<HkNodeBase>* child)
        {
            return total + child->getPayload()->node_.transformContext.scale.y;
        });

    const auto spaceLeft = thisTc_->scale.y - childrenHeight;

    auto startPosY = thisTc_->pos.y - thisTc_->scale.y / 2 + spaceLeft / 2;
    for (const auto& child : children)
    {
        auto& childTc = child->getPayload()->node_.transformContext;
        startPosY += childTc.scale.y / 2;
        childTc.setPos({ thisTc_->pos.x, startPosY });
        startPosY += childTc.scale.y / 2;
    }
}

void HkConstraintContext::alignHorizontally(const std::vector<HkTreeStructure<HkNodeBase>*>& children)
{
    /*
        Centers children horizontally inside the parent and places children one after the other horizontally.
        Parent's Y value is used for the children Y value.
    */
    const auto childrenWidth = std::accumulate(children.begin(), children.end(), 0,
        [](int total, HkTreeStructure<HkNodeBase>* child)
        {
            /* Skip ScrollBars from width calc */
            //TODO: Maybe introduce enum types so we dont compare strings each time?
            if (child->getType() == "ScrollBar") return total;
            return total + child->getPayload()->node_.transformContext.scale.x;
        });

    const auto spaceLeft = thisTc_->scale.x - childrenWidth;

    std::cout << glfwGetTime() << " width available: " << thisTc_->scale.x
        << " width calculated: " << childrenWidth
        << " diff calculated: " << spaceLeft << '\n';

    /* This should be it's separate function. This will be the effective trigger to tell if scrollbars
       are needed to scroll the content or not */
    (spaceLeft < 0) ? isOverflowX_ = true : isOverflowX_ = false;
    // (thisTc_->scale.x < childrenWidth) ? isOverflowY_ = true : isOverflowY_ = false;

    auto startPosX = thisTc_->pos.x - thisTc_->scale.x / 2 + spaceLeft / 2 - offsetPercentage_.x;
    for (const auto& child : children)
    {
        if (child->getType() == "ScrollBar") continue;

        auto& childTc = child->getPayload()->node_.transformContext;
        startPosX += childTc.scale.x / 2;
        childTc.setPos({ startPosX , thisTc_->pos.y });
        startPosX += childTc.scale.x / 2;
    }
}

/* This simply constraints the Knob inside the scrollbar itself, at given currentValue (0 to 1) */
void HkConstraintContext::constrainSBKnob(bool isFromHorizontalSB, float currKnobValue, HkTransformContext& knobTc)
{
    const auto knobSqSize = std::min(thisTc_->scale.x, thisTc_->scale.y);
    knobTc.setScale({ knobSqSize, knobSqSize });

    if (isFromHorizontalSB)
    {
        /* Orientation calcs do differ */
        const auto minX = thisTc_->pos.x - thisTc_->scale.x / 2 + knobSqSize / 2;
        const auto maxX = thisTc_->pos.x + thisTc_->scale.x / 2 - knobSqSize / 2;
        const auto posX = minX * (1.0f - currKnobValue) + currKnobValue * maxX;
        knobTc.setPos({ posX, thisTc_->pos.y });
    }
    else
    {
        const auto minY = thisTc_->pos.y - thisTc_->scale.y / 2 + knobSqSize / 2;
        const auto maxY = thisTc_->pos.y + thisTc_->scale.y / 2 - knobSqSize / 2;
        const auto posY = minY * (1.0f - currKnobValue) + currKnobValue * maxY;
        knobTc.setPos({ thisTc_->pos.x, posY });
    }
}

/* Keeps scrollbar object at the bottom or at the right of the container. */
void HkConstraintContext::scrollBarConstrain(HkTransformContext& scrollBarTc, bool isHorizontalBar)
{
    const auto barScale = 20;
    if (isHorizontalBar)
    {
        /* We should take into account if vertical bar is present so that bottom right 'intersection'
           between bars is filled or not. Same thing shoukd apply for vertical calcs bellow */
        const auto verticalBarAwareMargin = isOverflowY_ ? barScale : 0;
        scrollBarTc.setScale({ thisTc_->scale.x - verticalBarAwareMargin, barScale });
        scrollBarTc.setPos({ thisTc_->pos.x - verticalBarAwareMargin / 2, thisTc_->pos.y + thisTc_->scale.y / 2 - barScale / 2 });
    }
    else
    {
        const auto horizontalBarAwareMargin = isOverflowX_ ? barScale : 0;
        scrollBarTc.setScale({ barScale, thisTc_->scale.y - horizontalBarAwareMargin });
        scrollBarTc.setPos({ thisTc_->pos.x + thisTc_->scale.x / 2 - barScale / 2, thisTc_->pos.y - horizontalBarAwareMargin / 2 });
    }
}

/* WindowFrame is a special UI element that 'drags' a container along with it that sits underneath the window frame. This
   function helps constraint that container to the windowFrame element */
void HkConstraintContext::windowFrameContainerConstraint(HkTransformContext& childTc)
{
    /* TODO: dirty flags shall be used here to not do redundant repositioning */
    childTc.setPos({ thisTc_->pos.x, thisTc_->pos.y + childTc.scale.y / 2 + thisTc_->scale.y / 2 });
}

void HkConstraintContext::freeConstraint(const std::vector<HkTreeStructure<HkNodeBase>*>&)
{
    /*
        Centers children horizontally inside the parent and places children one after the other horizontally.
        Parent's Y value is used for the children Y value.
    */
    // const auto childrenWidth = std::accumulate(children.begin(), children.end(), 0,
    //     [](int total, HkTreeStructure<HkNodeBase>* child)
    //     {
    //         return total + child->getPayload()->node_.transformContext.scale.x;
    //     });

    // const auto spaceLeft = thisTc_->scale.x - childrenWidth;

    // auto startPosX = thisTc_->pos.x - thisTc_->scale.x / 2 + spaceLeft / 2;
    // for (const auto& child : children)
    // {
    //     auto& childTc = child->getPayload()->node_.transformContext;
    //     startPosX += childTc.scale.x / 2;
    //     childTc.setPos({ startPosX , thisTc_->pos.y });
    //     startPosX += childTc.scale.x / 2;
    // }
    // ---------------------------------------------------------------------------------------------------------
    // /*
    //     Centers children vertically inside the parent and places children one after the other vertically.
    //     Parent's X value is used for the children X value.
    // */
    // const auto childrenHeight = std::accumulate(children.begin(), children.end(), 0,
    //     [](int total, HkTreeStructure<HkNodeBase>* child)
    //     {
    //         return total + child->getPayload()->node_.transformContext.scale.y;
    //     });

    // const auto spaceLeft = thisTc_->scale.y - childrenHeight;

    // auto startPosY = thisTc_->pos.y - thisTc_->scale.y / 2 + spaceLeft / 2;
    // for (const auto& child : children)
    // {
    //     auto& childTc = child->getPayload()->node_.transformContext;
    //     startPosY += childTc.scale.y / 2;
    //     childTc.setPos({ thisTc_->pos.x, startPosY });
    //     startPosY += childTc.scale.y / 2;
    // }
}
} // hkui