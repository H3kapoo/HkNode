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
    //TODO: Maybe break into smaller functions? Will speed impact be negligeable?
    /* Extremely important to not do anything if element doesnt host any children */
    if (children.empty())
        return;
    computeScrollBarCount();


    int32_t startPosY = 0;
    int32_t startPosX = 0;

    bool rowWrapping = true;
    // bool rowWrapping = false;
    // int32_t rowIdToMaxHeight[10]; // max of 10 wrapped rows

    int32_t highestYOnRow = 0;
    uint32_t highestYOnRowId = 0;
    uint32_t nextRowFirstId = 0;
    uint32_t nextLastRowId = 0;
    for (uint32_t i = 0; i < children.size() - sbCount_; i++)
    {
        auto& child = children[i]->getPayload()->node_;
        auto& childTc = child.transformContext;
        auto& childCc = child.constraintContext;

        /*Horizontal container specific logic*/
        if (direction_ == HkDirection::Horizontal)
        {
            /* How much we need to advance to place next child */
            int32_t nextXAdvance = startPosX + childCc.styleParams_.marginLX + childCc.styleParams_.marginRX + childTc.getScale().x;
            if (rowWrapping)
            {
                /* This basically "spawns" a new row */
                if (nextXAdvance > thisTc_->getScale().x)
                {
                    // since we got here, its safe to say row ended before this. corner cases?
                    nextLastRowId = i - 1;

                    startPosX = 0;
                    startPosY += highestYOnRow;

                    /*Back Propagate child alignment now that we completed the row */
                    for (uint32_t j = nextRowFirstId; j <= nextLastRowId;j++)
                    {
                        /* Skip aligning the highest itself. Might not be needed tho */
                        if (j == highestYOnRowId) continue;
                        auto& childBpTc = children[j]->getPayload()->node_.transformContext;

                        childBpTc.addPos({ 0, highestYOnRow - childBpTc.getScale().y });
                    }

                    /* After back propagating, its the current element that starts the new row*/
                    nextRowFirstId = i;

                    nextXAdvance = childTc.getScale().x;
                    highestYOnRow = 0;
                }

                /* Needed so Y of wrapped child starts at lowest Y possible */
                const int32_t maybeHighest = childTc.getScale().y + childCc.styleParams_.marginBY;
                if (maybeHighest > highestYOnRow)
                {
                    highestYOnRow = maybeHighest;
                    highestYOnRowId = i;
                }
            }
            childTc.setPos({ startPosX + childCc.styleParams_.marginLX  , startPosY + childCc.styleParams_.marginTY });
            startPosX = nextXAdvance;

        }
    }

    // // propagate here instead?
    for (uint32_t i = nextRowFirstId; i < children.size() - sbCount_;i++)
    {
        /* Skip aligning the highest itself. Might not be needed tho */
        if (i == highestYOnRowId) continue;
        auto& childBpTc = children[i]->getPayload()->node_.transformContext;

        // bottom
        childBpTc.addPos({ 0, highestYOnRow - childBpTc.getScale().y });
    }

    MinMaxPos result = getMinAndMaxPositions(children); // we calculate the same thing later. optimize
    startPosX = 0;

    /* Calculate start point for placement on the X axis based on container options*/
    if (horizontalAlignment_ == HkAlignment::Left)
    {
        startPosX = thisTc_->getPos().x;
    }
    else if (horizontalAlignment_ == HkAlignment::Center)
    {
        startPosX = thisTc_->getPos().x + thisTc_->getScale().x / 2 - ((result.maxX - result.minX) / 2);
    }
    else if (horizontalAlignment_ == HkAlignment::Right)
    {
        startPosX = (thisTc_->getPos().x + thisTc_->getScale().x) - (result.maxX - result.minX);
    }

    for (uint32_t i = 0; i < children.size() - sbCount_; i++)
    {
        auto& child = children[i]->getPayload()->node_;
        auto& childTc = child.transformContext;
        //TODO: these margins,borders, padding etc shall be retrieved from style context, not constraint one
        auto& childCc = child.constraintContext;
        childTc.addPos({ startPosX , 0 });
    }



    // // int32_t startPosX = thisTc_->getPos().x;
    // int32_t startPosY = 0;//thisTc_->getPos().y;

    // /* Calculate start point for placement on the Y axis based on container options*/
    // // if (verticalAlignment_ == HkAlignment::Top)
    // // {
    // //     startPosY = thisTc_->getPos().y;
    // // }
    // // else if (verticalAlignment_ == HkAlignment::Center)
    // // {
    // //     startPosY = (thisTc_->getPos().y + thisTc_->getScale().y) / 2 - ((result.maxY - result.minY) / 2);
    // // }
    // // else if (verticalAlignment_ == HkAlignment::Bottom)
    // // {
    // //     startPosY = (thisTc_->getPos().y + thisTc_->getScale().y) - (result.maxY - result.minY);
    // // }

    // /* Place children accoring to their own alignment starting at above calculated positions*/
    // int32_t startPosX = 0;//thisTc_->getPos().x;

    // bool rowWrapping = true;
    // // bool rowWrapping = false;
    // int32_t highestYOnRow = 0;
    // for (uint32_t i = 0; i < children.size() - sbCount_; i++)
    // {
    //     auto& child = children[i]->getPayload()->node_;
    //     auto& childTc = child.transformContext;
    //     //TODO: these margins,borders, padding etc shall be retrieved from style context, not constraint one
    //     auto& childCc = child.constraintContext;

    //     /*Horizontal container specific logic*/
    //     if (direction_ == HkDirection::Horizontal)
    //     {
    //         int32_t nextXAdvance = 0;
    //         /* How much we need to advance to place next child */
    //         nextXAdvance = startPosX + childCc.styleParams_.marginLX + childCc.styleParams_.marginRX + childTc.getScale().x;
    //         if (rowWrapping)
    //         {
    //             //TODO: Account for scrollbars size as well somehow
    //             /* This basically "spawns" a new row */
    //             if (nextXAdvance > thisTc_->getScale().x)
    //             {
    //                 // MinMaxPos r2 = getMinAndMaxPositions(children); // we calculate the same thing later. optimize

    //                 startPosX = 0;
    //                 startPosY += highestYOnRow;
    //                 nextXAdvance = childTc.getScale().x;
    //                 highestYOnRow = 0;
    //             }
    //             /* Needed so Y of wrapped child starts at lowest Y possible */
    //             const int32_t maybeHighest = childTc.getScale().y + childCc.styleParams_.marginBY;
    //             highestYOnRow = maybeHighest > highestYOnRow ? maybeHighest : highestYOnRow;
    //         }

    //         if (childCc.verticalAlignment_ == HkAlignment::Top)
    //         {
    //             childTc.setPos({ startPosX + childCc.styleParams_.marginLX  , startPosY + childCc.styleParams_.marginTY });
    //         }
    //         // else if (childCc.verticalAlignment_ == HkAlignment::Center)
    //         // {
    //         //     // //TODO: Tis can be combined in one pass of setPos
    //         //     // childTc.setPos({ startPosX + childCc.styleParams_.marginLX , startPosY });
    //         //     // childTc.addPos({ 0 , ((result.maxY - result.minY) / 2) - childTc.getScale().y / 2 });
    //         // }
    //         else if (childCc.verticalAlignment_ == HkAlignment::Bottom)
    //         {
    //             childTc.setPos({ startPosX , startPosY });
    //             // childTc.addPos({ 0 , (result.maxY - result.minY) - childTc.getScale().y });
    //             // childTc.setPos({ startPosX + childCc.styleParams_.marginLX  , startPosY + childCc.styleParams_.marginTY });

    //         }

    //         startPosX = nextXAdvance;
    //     }
    //     /*Vertical container specific logic */
    //     else if (direction_ == HkDirection::Vertical)
    //     {
    //         // if (childCc.horizontalAlignment_ == HkAlignment::Left)
    //         // {
    //         //     childTc.setPos({ startPosX , startPosY });
    //         // }
    //         // else if (childCc.horizontalAlignment_ == HkAlignment::Center)
    //         // {
    //         //     childTc.setPos({ startPosX , startPosY });
    //         //     childTc.addPos({ ((result.maxX - result.minX) / 2) - childTc.getScale().x / 2 ,0 });
    //         // }
    //         // else if (childCc.horizontalAlignment_ == HkAlignment::Right)
    //         // {
    //         //     childTc.setPos({ startPosX , startPosY });
    //         //     childTc.addPos({ (result.maxX - result.minX) - childTc.getScale().x ,0 });
    //         // }
    //         // startPosY += childTc.getScale().y + childCc.styleParams_.marginTY;
    //     }
    // }

    // MinMaxPos result = getMinAndMaxPositions(children); // we calculate the same thing later. optimize
    // startPosX = 0;

    // /* Calculate start point for placement on the X axis based on container options*/
    // if (horizontalAlignment_ == HkAlignment::Left)
    // {
    //     startPosX = thisTc_->getPos().x;
    // }
    // else if (horizontalAlignment_ == HkAlignment::Center)
    // {
    //     startPosX = thisTc_->getPos().x + thisTc_->getScale().x / 2 - ((result.maxX - result.minX) / 2);
    // }
    // else if (horizontalAlignment_ == HkAlignment::Right)
    // {
    //     startPosX = (thisTc_->getPos().x + thisTc_->getScale().x) - (result.maxX - result.minX);
    // }

    // for (uint32_t i = 0; i < children.size() - sbCount_; i++)
    // {
    //     auto& child = children[i]->getPayload()->node_;
    //     auto& childTc = child.transformContext;
    //     //TODO: these margins,borders, padding etc shall be retrieved from style context, not constraint one
    //     auto& childCc = child.constraintContext;
    //     childTc.addPos({ startPosX , 0 });

    //     if (childCc.verticalAlignment_ == HkAlignment::Bottom)
    //     {
    //         // childTc.addPos({ 0 , (result.maxY - result.minY) - childTc.getScale().y });//(result.maxY - result.minY) - childTc.getScale().y });
    //     }
    //     // childTc.addPos({ 0 , (result.maxY - result.minY) - childTc.getScale().y });

    // }
    /*After positioning all children, calculate to see if we got an overflow on both axis */
    resolveAxisOverflow(children, sbSizes);
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

MinMaxScale HkConstraintContext::getMinAndMaxScale(const std::vector<HkTreeStructure<HkNodeBase>*>& children)
{
    // MinMaxScale result;
    // for (uint32_t i = 0; i < children.size() - sbCount_; i++)
    // {
    //     /* X related */
    //     const auto& childTc = children[i]->getPayload()->node_.transformContext;
    //     result.scaleX += childTc.getPos().x;
    //     result.scaleY += childTc.getPos().y;
    // }

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