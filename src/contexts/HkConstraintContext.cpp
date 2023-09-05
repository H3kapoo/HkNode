#include "HkConstraintContext.hpp"

#include "../base/HkNodeBase.hpp"
#include "../utils/HkDrawDebugger.hpp"

namespace hkui
{

void HkConstraintContext::setDirection(HkDirection dir) { direction_ = dir; }

void HkConstraintContext::setVAlignment(HkAlignment alignment) { verticalAlignment_ = alignment; }

void HkConstraintContext::setHAlignment(HkAlignment alignment) { horizontalAlignment_ = alignment; }

/* Set TC this ctx will be based on*/
void HkConstraintContext::setRootTc(HkTransformContext* rootTc) { thisTc_ = rootTc; }

/* Set policy of TC children */
void HkConstraintContext::setPolicy(const HkConstraintPolicy policy) { policy_ = policy; }

void HkConstraintContext::resolveConstraints(std::vector<HkTreeStructure<HkNodeBase>*>& children,
    const HkScrollbarsSize sbSizes)
{
    /* Extremely important to not do anything if element doesnt host any children */
    if (children.empty())
        return;
    computeScrollBarCount();

    if (direction_ == HkDirection::Horizontal)
        resolveHorizontalContainer(children, sbSizes);
    else
        resolveVerticalContainer(children, sbSizes);

    resolveAxisOverflow(children, sbSizes);
}

void HkConstraintContext::resolveHorizontalContainer(std::vector<HkTreeStructure<HkNodeBase>*>& children,
    const HkScrollbarsSize sbSizes)
{
    int32_t startPosX = 0, startPosY = 0;
    int32_t highestYOnRow = 0;
    uint32_t nextRowFirstId = 0;
    uint32_t lastRowEndId = 0;
    int32_t nextXAdvance = 0;
    int32_t maybeHighest = 0;
    bool rowWrapping = true; // make style
    for (uint32_t i = 0; i < children.size() - sbCount_; i++)
    {
        auto& child = children[i]->getPayload()->node_;
        auto& childTc = child.transformContext;
        auto& childCc = child.constraintContext;

        /* How much we need to advance to place next child */
        nextXAdvance = startPosX + childCc.styleParams_.marginLX + childCc.styleParams_.marginRX + childTc.getScale().x;
        if (rowWrapping)
        {
            /* This basically "spawns" a new row */
            if (nextXAdvance > thisTc_->getScale().x)
            {
                /* Since we got here, its safe to say row ended before this element */
                lastRowEndId = i - 1;
                startPosX = 0;
                startPosY += highestYOnRow;

                /*Back Propagate child alignment now that we completed the row */
                backPropagateRowChange(children, nextRowFirstId, lastRowEndId, highestYOnRow);

                /* After back propagating, its the current element that starts the new row*/
                nextRowFirstId = i;
                nextXAdvance = childCc.styleParams_.marginLX + childCc.styleParams_.marginRX + childTc.getScale().x;
                highestYOnRow = 0;
            }

            /* Needed so Y of wrapped child starts at lowest Y possible */
            maybeHighest = childTc.getScale().y + childCc.styleParams_.marginTY + childCc.styleParams_.marginBY;
            if (maybeHighest > highestYOnRow)
            {
                highestYOnRow = maybeHighest;
            }
        }
        else
        {
            /* Needed so Y of wrapped child starts at lowest Y possible */
            maybeHighest = childTc.getScale().y + childCc.styleParams_.marginTY + childCc.styleParams_.marginBY;
            if (maybeHighest > highestYOnRow)
            {
                highestYOnRow = maybeHighest;
            }
        }
        childTc.setPos({ startPosX + childCc.styleParams_.marginLX  , startPosY + childCc.styleParams_.marginTY });
        startPosX = nextXAdvance;
    }

    /*Back Propagate child alignment for the last row..or the only one row is no overflow occured with rowWrapping */
    backPropagateRowChange(children, nextRowFirstId, children.size() - sbCount_ - 1, highestYOnRow);

    /* Above calculations have been with respect to 0,0 ,so now we need to offset the generated "container"
       by the container's alignment type.
       This needs to be done here because otherwise we can get wrong min max information and bottom/center alignments
       need this information. It's a trade-off. */
    applyFinalOffsets(children);
}

void HkConstraintContext::resolveVerticalContainer(std::vector<HkTreeStructure<HkNodeBase>*>& children,
    const HkScrollbarsSize sbSizes)
{
    int32_t startPosX = 0, startPosY = 0;
    int32_t longestXOnCol = 0;
    uint32_t nextColFirstId = 0;
    uint32_t lastColEndId = 0;
    int32_t nextYAdvance = 0;
    int32_t maybeLongest = 0;
    bool colWrapping = true; // make style
    for (uint32_t i = 0; i < children.size() - sbCount_; i++)
    {
        auto& child = children[i]->getPayload()->node_;
        auto& childTc = child.transformContext;
        auto& childCc = child.constraintContext;

        /* How much we need to advance to place next child */
        nextYAdvance = startPosY + childCc.styleParams_.marginBY + childCc.styleParams_.marginTY + childTc.getScale().y;
        if (colWrapping)
        {
            /* This basically "spawns" a new col */
            if (nextYAdvance > thisTc_->getScale().y)
            {
                /* Since we got here, its safe to say row ended before this element */
                lastColEndId = i - 1;
                startPosX += longestXOnCol;
                startPosY = 0;

                /*Back Propagate child alignment now that we completed the col */

                backPropagateColChange(children, nextColFirstId, lastColEndId, longestXOnCol);

                /* After back propagating, its the current element that starts the new col*/
                nextColFirstId = i;
                nextYAdvance = childCc.styleParams_.marginLX + childCc.styleParams_.marginRX + childTc.getScale().y;
                longestXOnCol = 0;
            }

            /* Needed so Y of wrapped child starts at lowest Y possible */
            maybeLongest = childTc.getScale().x + childCc.styleParams_.marginLX + childCc.styleParams_.marginRX;
            if (maybeLongest > longestXOnCol)
            {
                longestXOnCol = maybeLongest;
            }
        }
        else
        {
            /* Needed so Y of wrapped child starts at lowest Y possible */
            maybeLongest = childTc.getScale().x + childCc.styleParams_.marginLX + childCc.styleParams_.marginRX;
            if (maybeLongest > longestXOnCol)
            {
                longestXOnCol = maybeLongest;
            }
        }

        childTc.setPos({ startPosX + childCc.styleParams_.marginLX  , startPosY + childCc.styleParams_.marginTY });
        startPosY = nextYAdvance;
    }

    /*Back Propagate child alignment for the last row..or the only one row is no overflow occured with rowWrapping */
    backPropagateColChange(children, nextColFirstId, children.size() - sbCount_ - 1, longestXOnCol);

    /* Above calculations have been with respect to 0,0 ,so now we need to offset the generated "container"
       by the container's alignment type.
       This needs to be done here because otherwise we can get wrong min max information and bottom/center alignments
       need this information. It's a trade-off. */
    applyFinalOffsets(children);
}


void HkConstraintContext::backPropagateRowChange(std::vector<HkTreeStructure<HkNodeBase>*>& children,
    const uint32_t nextRowFirstId, const uint32_t lastRowEndId, const uint32_t highestYOnRow)
{
    for (uint32_t j = nextRowFirstId; j <= lastRowEndId;j++)
    {
        auto& childBpTc = children[j]->getPayload()->node_.transformContext;
        auto& childBpCc = children[j]->getPayload()->node_.constraintContext;

        switch (childBpCc.verticalAlignment_) // here's wrong, it should be child's HA
        {
        case HkAlignment::Left:
        case HkAlignment::Right:
        case HkAlignment::Top:
            /*Valid, but do nothing */
            break;
        case HkAlignment::Center:
            childBpTc.addPos({ 0, highestYOnRow / 2 - childBpTc.getScale().y / 2 });
            break;
        case HkAlignment::Bottom:
            childBpTc.addPos({ 0, highestYOnRow - childBpTc.getScale().y });
            break;
        }
    }
}

void HkConstraintContext::backPropagateColChange(std::vector<HkTreeStructure<HkNodeBase>*>& children,
    const uint32_t nextColFirstId, const uint32_t lastColEndId, const uint32_t longestXOnCol)
{
    for (uint32_t j = nextColFirstId; j <= lastColEndId;j++)
    {
        auto& childBpTc = children[j]->getPayload()->node_.transformContext;
        auto& childBpCc = children[j]->getPayload()->node_.constraintContext;

        switch (childBpCc.horizontalAlignment_)
        {
        case HkAlignment::Top:
        case HkAlignment::Bottom:
        case HkAlignment::Left:
            /*Valid, but do nothing */
            break;
        case HkAlignment::Center:
            childBpTc.addPos({ longestXOnCol / 2 - childBpTc.getScale().x / 2, 0 });
            break;
        case HkAlignment::Right:
            childBpTc.addPos({ longestXOnCol - childBpTc.getScale().x, 0 });
            break;
        }
    }
}

void HkConstraintContext::applyFinalOffsets(std::vector<HkTreeStructure<HkNodeBase>*>& children)
{
    int32_t startPosX = 0, startPosY = 0;
    MinMaxPos result = getMinAndMaxPositions(children); // we calculate the same thing later. optimize

    /* Calculate start point for placement on the X axis based on container options*/
    switch (horizontalAlignment_)
    {
    case HkAlignment::Top:
    case HkAlignment::Bottom:
    case HkAlignment::Left:
        startPosX = thisTc_->getPos().x;
        break;
    case HkAlignment::Center:
        startPosX = thisTc_->getPos().x + thisTc_->getScale().x / 2 - ((result.maxX - result.minX) / 2);
        break;
    case HkAlignment::Right:
        startPosX = (thisTc_->getPos().x + thisTc_->getScale().x) - (result.maxX - result.minX);
        break;
    }

    /* Calculate start point for placement on the X axis based on container options*/
    switch (verticalAlignment_)
    {
    case HkAlignment::Left:
    case HkAlignment::Right:
    case HkAlignment::Top:
        startPosY = thisTc_->getPos().y;
        break;
    case HkAlignment::Center:
        startPosY = thisTc_->getPos().y + thisTc_->getScale().y / 2 - ((result.maxY - result.minY) / 2);
        break;
    case HkAlignment::Bottom:
        startPosY = (thisTc_->getPos().y + thisTc_->getScale().y) - (result.maxY - result.minY);
        break;
    }

    /* Finnaly offset the elements */
    for (uint32_t i = 0; i < children.size() - sbCount_; i++)
    {
        auto& child = children[i]->getPayload()->node_;
        auto& childTc = child.transformContext;
        //TODO: these margins,borders, padding etc shall be retrieved from style context, not constraint one
        // auto& childCc = child.constraintContext;
        childTc.addPos({ startPosX , startPosY });
    }
}

void HkConstraintContext::computeScrollBarCount()
{
    sbCount_ = 0;
    sbCount_ += (isOverflowAllowedX_ && overflowXYSize_.x) ? 1 : 0;
    sbCount_ += (isOverflowAllowedY_ && overflowXYSize_.y) ? 1 : 0;
}

void HkConstraintContext::computeChildrenOverflowBasedOnMinMax(const MinMaxPos& minMax,
    const HkScrollbarsSize sbSizes)
{
    /*Reset variables */
    isOverflowX_ = false;
    isOverflowY_ = false;
    overflowXYSize_.x = 0;
    overflowXYSize_.y = 0;

    const int32_t addedOverflowX = minMax.maxX - (thisTc_->getPos().x + thisTc_->getScale().x - sbSizes.vsbSize);
    const int32_t addedOverflowY = minMax.maxY - (thisTc_->getPos().y + thisTc_->getScale().y - sbSizes.hsbSize);
    /* Calculate X overflow */
    if (isOverflowAllowedX_)
    {
        if (minMax.maxX > thisTc_->getPos().x + thisTc_->getScale().x)
        {
            isOverflowX_ = true;
            overflowXYSize_.x = minMax.maxX - (thisTc_->getPos().x + thisTc_->getScale().x);
        }

        if (minMax.minX < thisTc_->getPos().x)
        {
            isOverflowX_ = true;
            overflowXYSize_.x += thisTc_->getPos().x - minMax.minX;
        }
    }

    /* Calculate Y overflow */
    if (isOverflowAllowedY_)
    {
        if (minMax.maxY > thisTc_->getPos().y + thisTc_->getScale().y)
        {
            isOverflowY_ = true;
            overflowXYSize_.y = minMax.maxY - (thisTc_->getPos().y + thisTc_->getScale().y);
        }

        if (minMax.minY < thisTc_->getPos().y)
        {
            isOverflowY_ = true;
            overflowXYSize_.y += thisTc_->getPos().y - minMax.minY;
        }
    }

    /* Treat case when we have overflow on both X and Y. In that case, we need to increase overflow size
       by how much each axis overflow and at most by the scrollbar size */
    if (isOverflowAllowedX_ && isOverflowAllowedY_ && addedOverflowX > 0 && addedOverflowY > 0)
    {
        overflowXYSize_.x += addedOverflowX > sbSizes.vsbSize ? sbSizes.vsbSize : addedOverflowX; //scrollBarMargins.hsbMargin;
        overflowXYSize_.y += addedOverflowY > sbSizes.hsbSize ? sbSizes.hsbSize : addedOverflowY; //scrollBarMargins.hsbMargin;
        isOverflowX_ = true;
        isOverflowY_ = true;
    }
    else
    {
        /* Treat cases where there's only one sb who can maybe overflow */
        if (isOverflowAllowedX_ && isOverflowY_ && addedOverflowX > 0)
        {
            overflowXYSize_.x += addedOverflowX > sbSizes.vsbSize ? sbSizes.vsbSize : addedOverflowX; //scrollBarMargins.hsbMargin;
            isOverflowX_ = true;
        }
        if (isOverflowAllowedY_ && isOverflowX_ && addedOverflowY > 0)
        {
            overflowXYSize_.y += addedOverflowY > sbSizes.hsbSize ? sbSizes.hsbSize : addedOverflowY; //scrollBarMargins.hsbMargin;
            isOverflowY_ = true;
        }
    }
}

void HkConstraintContext::resolveAxisOverflow(const std::vector<HkTreeStructure<HkNodeBase>*>& children,
    const HkScrollbarsSize sbSizes)
{
    MinMaxPos result = getMinAndMaxPositions(children);
    computeChildrenOverflowBasedOnMinMax(result, sbSizes);
    for (uint32_t i = 0; i < children.size() - sbCount_; i++)
    {
        auto& childTc = children[i]->getPayload()->node_.transformContext;
        const auto leftOverflow = (result.minX < thisTc_->getPos().x) ? thisTc_->getPos().x - result.minX : 0;
        const auto topOverflow = (result.minY < thisTc_->getPos().y) ? thisTc_->getPos().y - result.minY : 0;
        childTc.addPos({ leftOverflow + offsetPercentage_.x * -overflowXYSize_.x,
            topOverflow + offsetPercentage_.y * -overflowXYSize_.y, });
    }
}

MinMaxPos HkConstraintContext::getMinAndMaxPositions(const std::vector<HkTreeStructure<HkNodeBase>*>& children)
{
    MinMaxPos result;
    for (uint32_t i = 0; i < children.size() - sbCount_; i++)
    {
        /* X related */
        const auto& childTc = children[i]->getPayload()->node_.transformContext;
        if (childTc.getPos().x <= result.minX)
        {
            result.minX = childTc.getPos().x;
        }

        if (childTc.getPos().x + childTc.getScale().x >= result.maxX)
        {
            result.maxX = childTc.getPos().x + childTc.getScale().x;
        }

        /* Y related */
        if (childTc.getPos().y <= result.minY)
        {
            result.minY = childTc.getPos().y;
        }

        if (childTc.getPos().y + childTc.getScale().y >= result.maxY)
        {
            result.maxY = childTc.getPos().y + childTc.getScale().y;
        }
    }

    //Position dependent
    // for (uint32_t i = 0; i < children.size() - sbCount_; i++)
    // {
    //     /* X related */
    //     const auto& childTc = children[i]->getPayload()->node_.transformContext;
    //     if (childTc.getPos().x <= result.minX)
    //     {
    //         result.minX = childTc.getPos().x;
    //     }

    //     if (childTc.getPos().x + childTc.getScale().x >= result.maxX)
    //     {
    //         result.maxX = childTc.getPos().x + childTc.getScale().x;
    //     }

    //     /* Y related */
    //     if (childTc.getPos().y <= result.minY)
    //     {
    //         result.minY = childTc.getPos().y;
    //     }

    //     if (childTc.getPos().y + childTc.getScale().y >= result.maxY)
    //     {
    //         result.maxY = childTc.getPos().y + childTc.getScale().y;
    //     }
    // }

    // const auto& childTc = children[0]->getPayload()->node_.transformContext;
    // if (children.size() > 3)
    // {

    //     HkDrawDebugger::get().pushDraw10x10({ result.maxX - 3 + childTc.getPos().x ,300 });
    //     HkDrawDebugger::get().pushDraw10x10({ result.minX - 3 + childTc.getPos().x,300 });
    //     std::cout << "max :" << result.maxX - 3 + childTc.getPos().x
    //         << "min : " << result.minX - 3 + childTc.getPos().x << "\n";

    // }

    return result;
}

void HkConstraintContext::alignEvenTopToBottom(const std::vector<HkTreeStructure<HkNodeBase>*>&)
{
    // computeScrollBarCount();

    // const auto slotSize = thisTc_->getScale().y / (children.size() - sbCount_);
    // auto startPosX = thisTc_->getPos().x;
    // auto startPosY = thisTc_->getPos().y + slotSize / 2;

    // for (uint32_t i = 0; i < children.size() - sbCount_; i++)
    // {
    //     auto& childTc = children[i]->getPayload()->node_.transformContext;
    //     childTc.setPos({
    //         lockXAxis_ ? childTc.getPos().x : startPosX,
    //         startPosY + slotSize * i - childTc.getScale().y / 2 });
    // }

    // /* Compute scrollbars count first as this can influence the result */
    // computeScrollBarCount();
}


void HkConstraintContext::alignCenterLeftRight(const std::vector<HkTreeStructure<HkNodeBase>*>&)
{
    // const auto maxAndTotalHori = getHorizontalMaxValueAndTotalWidthValue(children);
    // // const auto slotSize = (thisTc_->getScale().x + overflowXYSize_.x) / children.size();
    // // const auto slotSize = (thisTc_->getScale().x) / children.size();
    // const auto center = thisTc_->getScale().x / 2;
    // const auto leftStart = center - maxAndTotalHori.total / 2;
    // // const auto rightStart = center + maxAndTotalHori.total / 2;

    // auto startPosX = leftStart + offsetPercentage_.x * -overflowXYSize_.x;
    // auto startPosY = thisTc_->getPos().y + offsetPercentage_.y * -overflowXYSize_.y;

    // //TODO: overflow due to repositioning of children does not occur
    // //TODO: need new method to calculate if theres an overflow. Position should be taken into account as well
    // // HkDrawDebugger::get().pushDraw10x10({
    // //     center,
    // //     300 });
    // // HkDrawDebugger::get().pushDraw10x10({
    // //     leftStart,
    // //     300 });
    // // HkDrawDebugger::get().pushDraw10x10({
    // //     rightStart,
    // //     300 });

    // for (const auto& child : children)
    // {
    //     if (child->getType() == "ScrollBar") continue;

    //     auto& childTc = child->getPayload()->node_.transformContext;
    //     childTc.setPos({ startPosX , startPosY });
    //     startPosX += childTc.getScale().x;
    // }
}

void HkConstraintContext::alignEvenLeftRight(const std::vector<HkTreeStructure<HkNodeBase>*>&)
{
    // computeScrollBarCount();

    // const auto slotSize = thisTc_->getScale().x / (children.size() - sbCount_);
    // auto startPosX = thisTc_->getPos().x + slotSize / 2;
    // auto startPosY = thisTc_->getPos().y;

    // for (uint32_t i = 0; i < children.size() - sbCount_; i++)
    // {
    //     auto& childTc = children[i]->getPayload()->node_.transformContext;
    //     childTc.setPos({
    //         startPosX + slotSize * i - childTc.getScale().x / 2,
    //         lockYAxis_ ? childTc.getPos().y : startPosY });
    // }
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