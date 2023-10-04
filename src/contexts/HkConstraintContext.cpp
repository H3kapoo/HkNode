#include "HkConstraintContext.hpp"

#include "../base/HkNodeBase.hpp"
#include "../utils/HkDrawDebugger.hpp"

namespace hkui
{

/* Set TC this ctx will be based on*/
void HkConstraintContext::injectTransformContext(HkTransformContext* styleContext) { thisTc_ = styleContext; }

void HkConstraintContext::resolveConstraints(HkTreeStruct& children,
    const HkScrollbarsSize sbSizes)
{
    /* Extremely important to not do anything if element doesnt host any children */
    if (children.empty())
        return;
    computeScrollBarCount();

    if (styleContextInj_->getLayout() == HkLayout::Horizontal)
        resolveHorizontalContainer(children, sbSizes);
    else if (styleContextInj_->getLayout() == HkLayout::Vertical)
        resolveVerticalContainer(children, sbSizes);
    else if (styleContextInj_->getLayout() == HkLayout::Grid)
        resolveGridContainer(children, sbSizes);

    // resolveAxisOverflow(children, sbSizes);
}

void HkConstraintContext::resolveGridContainer(HkTreeStruct& children,
    const HkScrollbarsSize)
{
    /* Floats are needed here because of GridConfig containing fractional parts itself.
       Without floats we losing precision in placing elements */
    float startPosX = 0, startPosY = 0;

    const uint32_t childrenCount = children.size() - sbCount_;
    const auto gridConfig = styleContextInj_->getGridConfig();
    const float colEqualPart = 1.0f / std::accumulate(gridConfig.cols.begin(), gridConfig.cols.end(), 0.0f);
    const float rowEqualPart = 1.0f / std::accumulate(gridConfig.rows.begin(), gridConfig.rows.end(), 0.0f);
    const uint32_t gridConfigColsSize = gridConfig.cols.size();
    const uint32_t gridConfigRowsSize = gridConfig.rows.size();


    for (uint32_t i = 0; i < childrenCount; i++)
    {
        auto& child = children[i]->getPayload()->node_;
        auto& childTc = child.transformContext;
        auto& childSc = child.styleContext;

        /* Return on possibly invalid index access*/
        if (childSc.getGridCol() > gridConfigColsSize || childSc.getGridCol() < 1 ||
            childSc.getGridRow() > gridConfigRowsSize || childSc.getGridRow() < 1)
        {
            return;
        }

        /* Deduce, fractionally, how much we should advance to place the element*/
        float xAdvanceFraction = 0.0f;
        for (uint32_t j = 0; j < childSc.getGridCol() - 1; j++)
        {
            xAdvanceFraction += (gridConfig.cols[j] * colEqualPart);
        }

        float yAdvanceFraction = 0.0f;
        for (uint32_t j = 0; j < childSc.getGridRow() - 1; j++)
        {
            yAdvanceFraction += (gridConfig.rows[j] * rowEqualPart);
        }


        /* By default, elements will be left aligned. When Center/Right alignment is chosen, we need to
           advance fractionally a little bit more to the right in order to put elements at the right place.
           This additional advance is half for Center and full for Right*/
        switch (childSc.getHAlignment())
        {
        case HkHAlignment::Left:
        {
            startPosX = xAdvanceFraction * (float)thisTc_->getScale().x;
        }
        break;
        case HkHAlignment::Center:
        {
            const float xAdvanceToCenter = (gridConfig.cols[childSc.getGridCol() - 1] * colEqualPart) * 0.5f;
            startPosX = (xAdvanceFraction + xAdvanceToCenter) * (float)thisTc_->getScale().x - childTc.getScale().x * 0.5f;
        }
        break;
        case HkHAlignment::Right:
        {
            const float xAdvanceToRight = (gridConfig.cols[childSc.getGridCol() - 1] * colEqualPart);
            startPosX = (xAdvanceFraction + xAdvanceToRight) * (float)thisTc_->getScale().x - childTc.getScale().x;
        }
        break;
        }

        /* By default, elements will be top aligned. When Center/Bottom alignment is chosen, we need to
           advance fractionally a little bit more to the bottom in order to put elements at the right place.
           This additional advance is half for Center and full for Bottom*/
        switch (childSc.getVAlignment())
        {
        case HkVAlignment::Top:
        {
            startPosY = yAdvanceFraction * (float)thisTc_->getScale().y;
        }
        break;
        case HkVAlignment::Center:
        {
            const float yAdvanceToCenter = (gridConfig.rows[childSc.getGridRow() - 1] * rowEqualPart) * 0.5f;
            startPosY = (yAdvanceFraction + yAdvanceToCenter) * (float)thisTc_->getScale().y - childTc.getScale().y * 0.5f;
        }
        break;
        case HkVAlignment::Bottom:
        {
            const float yAdvanceToRight = (gridConfig.rows[childSc.getGridRow() - 1] * rowEqualPart);
            startPosY = (yAdvanceFraction + yAdvanceToRight) * (float)thisTc_->getScale().y - childTc.getScale().y;
        }
        break;
        }

        childTc.setPos({ startPosX, startPosY });
    }
}

void HkConstraintContext::resolveHorizontalContainer(HkTreeStruct& children,
    const HkScrollbarsSize) //TODO: Take into account sbSize in the future
{
    int32_t startPosX = 0, startPosY = 0;
    int32_t highestYOnRow = 0;
    uint32_t nextRowFirstId = 0;
    uint32_t lastRowEndId = 0;
    int32_t nextXAdvance = 0;
    int32_t maybeHighest = 0;
    for (uint32_t i = 0; i < children.size() - sbCount_; i++)
    {
        auto& child = children[i]->getPayload()->node_;
        auto& childTc = child.transformContext;
        auto& childSc = child.styleContext;

        /* How much we need to advance to place next child */
        nextXAdvance = startPosX + childSc.getLeftMargin() + childSc.getRightMargin() + childTc.getScale().x;
        if (styleContextInj_->isRowWrappingEnabled())
        {
            /* This basically "spawns" a new row */
            if (nextXAdvance > thisTc_->getScale().x)
            {
                /* Since we got here, its safe to say row ended before this element */
                lastRowEndId = i == 0 ? 0 : i - 1; // prevent underflow

                startPosX = 0;
                startPosY += highestYOnRow;

                /*Back Propagate child alignment now that we completed the row */
                backPropagateRowChange(children, nextRowFirstId, lastRowEndId, highestYOnRow);

                /* After back propagating, its the current element that starts the new row*/
                nextRowFirstId = i;
                nextXAdvance = childSc.getLeftMargin() + childSc.getRightMargin() + childTc.getScale().x;
                highestYOnRow = 0;
            }

            /* Needed so Y of wrapped child starts at lowest Y possible */
            maybeHighest = childTc.getScale().y + childSc.getTopMargin() + childSc.getBottomMargin();
            if (maybeHighest > highestYOnRow)
            {
                highestYOnRow = maybeHighest;
            }
        }
        else
        {
            /* Needed so Y of wrapped child starts at lowest Y possible */
            maybeHighest = childTc.getScale().y + childSc.getTopMargin() + childSc.getBottomMargin();
            if (maybeHighest > highestYOnRow)
            {
                highestYOnRow = maybeHighest;
            }
        }
        childTc.setPos({ startPosX + childSc.getLeftMargin(), startPosY + childSc.getTopMargin() });
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

void HkConstraintContext::resolveVerticalContainer(HkTreeStruct& children,
    const HkScrollbarsSize) //TODO: Take into account sbSize in the future
{
    //TODO: move into hpp so we dont redeclare them each frame
    int32_t startPosX = 0, startPosY = 0;
    int32_t longestXOnCol = 0;
    uint32_t nextColFirstId = 0;
    uint32_t lastColEndId = 0;
    int32_t nextYAdvance = 0;
    int32_t maybeLongest = 0;
    for (uint32_t i = 0; i < children.size() - sbCount_; i++)
    {
        auto& child = children[i]->getPayload()->node_;
        auto& childTc = child.transformContext;
        auto& childSc = child.styleContext;

        /* How much we need to advance to place next child */
        nextYAdvance = startPosY + childSc.getBottomMargin() + childSc.getTopMargin() + childTc.getScale().y; //TODO: Minimize this call
        if (styleContextInj_->isColWrappingEnabled())
        {
            /* This basically "spawns" a new col */
            if (nextYAdvance > thisTc_->getScale().y)
            {
                /* Since we got here, its safe to say row ended before this element */
                lastColEndId = i == 0 ? 0 : i - 1; // prevent underflow

                startPosX += longestXOnCol;
                startPosY = 0;

                /*Back Propagate child alignment now that we completed the col */

                backPropagateColChange(children, nextColFirstId, lastColEndId, longestXOnCol);

                /* After back propagating, its the current element that starts the new col*/
                nextColFirstId = i;
                nextYAdvance = childSc.getLeftMargin() + childSc.getRightMargin() + childTc.getScale().y;
                longestXOnCol = 0;
            }

            /* Needed so Y of wrapped child starts at lowest Y possible */
            maybeLongest = childTc.getScale().x + childSc.getLeftMargin() + childSc.getRightMargin();
            if (maybeLongest > longestXOnCol)
            {
                longestXOnCol = maybeLongest;
            }
        }
        else
        {
            /* Needed so Y of wrapped child starts at lowest Y possible */
            maybeLongest = childTc.getScale().x + childSc.getLeftMargin() + childSc.getRightMargin();
            if (maybeLongest > longestXOnCol)
            {
                longestXOnCol = maybeLongest;
            }
        }

        childTc.setPos({ startPosX + childSc.getLeftMargin()  , startPosY + childSc.getTopMargin() });
        startPosY = nextYAdvance;
    }

    /*Back Propagate child alignment for the last row..or the only one row if no overflow occured with rowWrapping */
    backPropagateColChange(children, nextColFirstId, children.size() - sbCount_ - 1, longestXOnCol);

    /* Above calculations have been with respect to 0,0 ,so now we need to offset the generated "container"
       by the container's alignment type.
       This needs to be done here because otherwise we can get wrong min max information and bottom/center alignments
       need this information. It's a trade-off. */
    applyFinalOffsets(children);
}


void HkConstraintContext::backPropagateRowChange(HkTreeStruct& children,
    const uint32_t nextRowFirstId, const uint32_t lastRowEndId, const uint32_t highestYOnRow) const
{
    for (uint32_t j = nextRowFirstId; j <= lastRowEndId;j++)
    {
        auto& childBpTc = children[j]->getPayload()->node_.transformContext; //TODO: Minimize this call
        auto& childBpSc = children[j]->getPayload()->node_.styleContext;

        switch (childBpSc.getVAlignment())
        {
        case HkVAlignment::Top:
            /*Valid, but do nothing */
            break;
        case HkVAlignment::Center:
            childBpTc.addPos({ 0, highestYOnRow / 2 - childBpTc.getScale().y / 2 });
            break;
        case HkVAlignment::Bottom:
            childBpTc.addPos({ 0, highestYOnRow - childBpTc.getScale().y });
            break;
        }
    }
}

void HkConstraintContext::backPropagateColChange(HkTreeStruct& children,
    const uint32_t nextColFirstId, const uint32_t lastColEndId, const uint32_t longestXOnCol) const
{
    for (uint32_t j = nextColFirstId; j <= lastColEndId;j++)
    {
        auto& childBpTc = children[j]->getPayload()->node_.transformContext;
        auto& childBpSc = children[j]->getPayload()->node_.styleContext;

        switch (childBpSc.getHAlignment())
        {
        case HkHAlignment::Left:
            /*Valid, but do nothing */
            break;
        case HkHAlignment::Center:
            childBpTc.addPos({ longestXOnCol / 2 - childBpTc.getScale().x / 2, 0 });
            break;
        case HkHAlignment::Right:
            childBpTc.addPos({ longestXOnCol - childBpTc.getScale().x, 0 });
            break;
        }
    }
}

void HkConstraintContext::applyFinalOffsets(HkTreeStruct& children) const
{
    int32_t startPosX = 0, startPosY = 0;
    MinMaxPos result = getMinAndMaxPositions(children); // we calculate the same thing later. optimize

    /* Calculate start point for placement on the X axis based on container options*/
    switch (styleContextInj_->getHAlignment())
    {
    case HkHAlignment::Left:
        startPosX = thisTc_->getPos().x;
        break;
    case HkHAlignment::Center:
        startPosX = thisTc_->getPos().x + thisTc_->getScale().x / 2 - ((result.maxX - result.minX) / 2);
        break;
    case HkHAlignment::Right:
        startPosX = (thisTc_->getPos().x + thisTc_->getScale().x) - (result.maxX - result.minX);
        break;
    }

    /* Calculate start point for placement on the X axis based on container options*/
    switch (styleContextInj_->getVAlignment())
    {
    case HkVAlignment::Top:
        startPosY = thisTc_->getPos().y;
        break;
    case HkVAlignment::Center:
        startPosY = thisTc_->getPos().y + thisTc_->getScale().y / 2 - ((result.maxY - result.minY) / 2);
        break;
    case HkVAlignment::Bottom:
        startPosY = (thisTc_->getPos().y + thisTc_->getScale().y) - (result.maxY - result.minY);
        break;
    }

    /* Finnaly offset the elements */
    for (uint32_t i = 0; i < children.size() - sbCount_; i++)
    {
        auto& child = children[i]->getPayload()->node_;
        auto& childTc = child.transformContext;
        childTc.addPos({ startPosX , startPosY });
    }
}

void HkConstraintContext::computeScrollBarCount()
{
    sbCount_ = 0;
    sbCount_ += (styleContextInj_->isOverflowAllowedX() && overflowXYSize_.x) ? 1 : 0;
    sbCount_ += (styleContextInj_->isOverflowAllowedY() && overflowXYSize_.y) ? 1 : 0;
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
    if (styleContextInj_->isOverflowAllowedX())
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
    if (styleContextInj_->isOverflowAllowedY())
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
    if (styleContextInj_->isOverflowAllowedX() && styleContextInj_->isOverflowAllowedY() && addedOverflowX > 0 && addedOverflowY > 0)
    {
        overflowXYSize_.x += addedOverflowX > sbSizes.vsbSize ? sbSizes.vsbSize : addedOverflowX; //scrollBarMargins.hsbMargin;
        overflowXYSize_.y += addedOverflowY > sbSizes.hsbSize ? sbSizes.hsbSize : addedOverflowY; //scrollBarMargins.hsbMargin;
        isOverflowX_ = true;
        isOverflowY_ = true;
    }
    else
    {
        /* Treat cases where there's only one sb who can maybe overflow */
        if (styleContextInj_->isOverflowAllowedX() && isOverflowY_ && addedOverflowX > 0)
        {
            overflowXYSize_.x += addedOverflowX > sbSizes.vsbSize ? sbSizes.vsbSize : addedOverflowX; //scrollBarMargins.hsbMargin;
            isOverflowX_ = true;
        }
        if (styleContextInj_->isOverflowAllowedY() && isOverflowX_ && addedOverflowY > 0)
        {
            overflowXYSize_.y += addedOverflowY > sbSizes.hsbSize ? sbSizes.hsbSize : addedOverflowY; //scrollBarMargins.hsbMargin;
            isOverflowY_ = true;
        }
    }
}

void HkConstraintContext::resolveAxisOverflow(const HkTreeStruct& children,
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

MinMaxPos HkConstraintContext::getMinAndMaxPositions(const HkTreeStruct& children) const
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
    return result;
}


/* This simply constraints the Knob inside the scrollbar itself, at given currentValue (0 to 1) */
void HkConstraintContext::constrainSBKnob(bool isFromHorizontalSB, int overflowSize, float currKnobValue,
    HkTransformContext& knobTc) const
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
void HkConstraintContext::scrollBarConstrain(HkTransformContext& scrollBarTc) const
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
    /* Necessary so we still "render" the frame in renderMySelf scissor pass */
    thisTc_->setScale({ -1,-1 });

    wfCtr.setScale(windowSize + 1); //TODO: hacK: looks like wfCtr gets a windowSize thats lacking exactly 1px behind (looks like GPU dependent)
    wfCtr.setPos({ 0,0 });
}

void HkConstraintContext::injectStyleContext(HkStyleContext* styleContext)
{
    styleContextInj_ = styleContext;
}

} // hkui