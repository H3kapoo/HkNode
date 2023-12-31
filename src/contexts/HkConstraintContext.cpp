#include "HkConstraintContext.hpp"

#include <algorithm>

#include "../base/HkNodeData.hpp"
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

    const auto layout = styleContextInj_->getLayout();

    switch (layout)
    {
    case HkLayout::Horizontal:
        resolveHorizontalContainer(children);
        break;
    case HkLayout::HPinch:
        resolveHorizontalPinch(children);
        break;
    case HkLayout::Vertical:
        resolveVerticalContainer(children);
        break;
    case HkLayout::VPinch:
        resolveVerticalPinch(children);
        break;
    case HkLayout::Grid:
        resolveGridContainer(children);
        break;
    }

    resolveAxisOverflow(children, sbSizes);
}

void HkConstraintContext::resolveGridContainer(HkTreeStruct& children)
{
    /* Floats are needed here because of GridConfig containing fractional parts itself.
       Without floats we losing precision in placing elements */
    float startPosX = 0, startPosY = 0;
    float xSize = 0, ySize = 0;

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
        auto& childBTc = child.borderTransformContext;
        auto& childSc = child.styleContext;

        /* Return on possibly invalid index access*/
        //TODO: Later we shall log the errors if any
        if (childSc.getGridCol() - 1 > gridConfigColsSize || childSc.getGridCol() < 1 ||
            childSc.getGridRow() - 1 > gridConfigRowsSize || childSc.getGridRow() < 1)
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

        const int32_t bX = childSc.getRightBorder() + childSc.getLeftBorder();
        const int32_t bY = childSc.getTopBorder() + childSc.getBottomBorder();
        const int32_t bmX = bX + childSc.getRightMargin() + childSc.getLeftMargin();
        const int32_t bmY = bY + childSc.getTopMargin() + childSc.getBottomMargin();

        const int32_t lPadd = styleContextInj_->getLeftPadding();
        const int32_t rPadd = styleContextInj_->getRightPadding();
        const int32_t tPadd = styleContextInj_->getTopPadding();
        const int32_t bPadd = styleContextInj_->getBottomPadding();

        const float xScalePadded = thisTc_->getContentScale().x - (rPadd + lPadd);
        const float xPosPadded = thisTc_->getContentPos().x + lPadd;

        const float yScalePadded = thisTc_->getContentScale().y - (bPadd + tPadd);
        const float yPosPadded = thisTc_->getContentPos().y + tPadd;

        /* Scale elements according to their config*/
        const auto hSizeConfig = childSc.getHSizeConfig();
        const auto vSizeConfig = childSc.getVSizeConfig();
        switch (hSizeConfig.type)
        {
        case HkSizeType::Absolute:
            xSize = hSizeConfig.value;
            break;
        case HkSizeType::PercCell:
            xSize = (gridConfig.cols[childSc.getGridCol() - 1] * colEqualPart) * xScalePadded;
            xSize *= hSizeConfig.value;
            xSize -= bX;
            break;
        case HkSizeType::FitCell:
            xSize = (gridConfig.cols[childSc.getGridCol() - 1] * colEqualPart) * xScalePadded;
            xSize -= bX;
            break;
        case HkSizeType::PercParent:
        case HkSizeType::FitParent:
        case HkSizeType::Balanced:
            /* Fall through, unsupported mode by grid*/
            xSize = 20;
            break;
        }

        switch (vSizeConfig.type)
        {
        case HkSizeType::Absolute:
            ySize = vSizeConfig.value;
            break;
        case HkSizeType::PercCell:
            ySize = (gridConfig.rows[childSc.getGridRow() - 1] * rowEqualPart) * yScalePadded;
            ySize *= vSizeConfig.value;
            ySize -= bY;
            break;
        case HkSizeType::FitCell:
            ySize = (gridConfig.rows[childSc.getGridRow() - 1] * rowEqualPart) * yScalePadded;
            ySize -= bY;
            break;
        case HkSizeType::PercParent:
        case HkSizeType::FitParent:
        case HkSizeType::Balanced:
            /* Fall through, unsupported mode by grid*/
            ySize = 20;
            break;
        }

        xSize = std::clamp(xSize, hSizeConfig.min, hSizeConfig.max);
        ySize = std::clamp(ySize, vSizeConfig.min, vSizeConfig.max);

        /*Ceil is needed so we don't get off by one pixel artifacts*/
        childTc.setContentScale({ ceil(xSize), ceil(ySize) });
        childTc.setScale({ childTc.getContentScale().x + bmX, childTc.getContentScale().y + bmY });
        childBTc.setContentScale({ childTc.getContentScale().x + bX, childTc.getContentScale().y + bY });

        /* By default, elements will be left aligned. When Center/Right alignment is chosen, we need to
           advance fractionally a little bit more to the right in order to put elements at the right place.
           This additional advance is half for Center and full for Right*/
        switch (childSc.getHAlignment())
        {
        case HkHAlignment::Left:
        {
            startPosX = xAdvanceFraction * xScalePadded;
            break;
        }
        case HkHAlignment::Center:
        {
            const float xAdvanceToCenter = (gridConfig.cols[childSc.getGridCol() - 1] * colEqualPart) * 0.5f;
            startPosX = (xAdvanceFraction + xAdvanceToCenter) * xScalePadded - childTc.getContentScale().x * 0.5f;
            break;
        }
        case HkHAlignment::Right:
        {
            const float xAdvanceToRight = (gridConfig.cols[childSc.getGridCol() - 1] * colEqualPart);
            startPosX = (xAdvanceFraction + xAdvanceToRight) * xScalePadded - childTc.getContentScale().x;
            break;
        }
        }

        /* By default, elements will be top aligned. When Center/Bottom alignment is chosen, we need to
           advance fractionally a little bit more to the bottom in order to put elements at the right place.
           This additional advance is half for Center and full for Bottom*/
        switch (childSc.getVAlignment())
        {
        case HkVAlignment::Top:
        {
            startPosY = yAdvanceFraction * yScalePadded;
            break;
        }
        case HkVAlignment::Center:
        {
            const float yAdvanceToCenter = (gridConfig.rows[childSc.getGridRow() - 1] * rowEqualPart) * 0.5f;
            startPosY = (yAdvanceFraction + yAdvanceToCenter) * yScalePadded - childTc.getContentScale().y * 0.5f;
            break;
        }
        case HkVAlignment::Bottom:
        {
            const float yAdvanceToRight = (gridConfig.rows[childSc.getGridRow() - 1] * rowEqualPart);
            startPosY = (yAdvanceFraction + yAdvanceToRight) * yScalePadded - childTc.getContentScale().y;
            break;
        }
        }

        /* Set absolute position of element */
        childTc.setPos({ startPosX + xPosPadded,  startPosY + yPosPadded });

        /* Set element's contentPos taking into account BM*/
        childTc.setContentPos(
            {
                childTc.getPos().x + childSc.getLeftMargin() + childSc.getLeftBorder(),
                childTc.getPos().y + childSc.getTopMargin() + childSc.getTopBorder()
            });

        /* Set element's border contentPos taking into account PB. We don't care about absolute pos */
        childBTc.setContentPos(
            {
                childTc.getContentPos().x - childSc.getLeftBorder(),
                childTc.getContentPos().y - childSc.getTopBorder()
            });
    }
}

void HkConstraintContext::resolveHorizontalContainer(HkTreeStruct& children)
{
    int32_t startPosX = 0, startPosY = 0;
    int32_t highestYOnRow = 0;
    int32_t nextXAdvance = 0;
    uint32_t rowFirstId = 0;
    uint32_t rowLastId = 0;
    uint32_t childCount = children.size() - sbCount_;

    int32_t combinedPaddingX = 0;
    int32_t combinedPaddingY = 0;
    for (uint32_t i = 0; i < childCount; i++)
    {
        auto& child = children[i]->getPayload()->node_;
        auto& childSc = child.styleContext;

        combinedPaddingX += childSc.getRightBorder() + childSc.getLeftBorder()
            + childSc.getRightMargin() + childSc.getLeftMargin();
        combinedPaddingY += childSc.getTopBorder() + childSc.getBottomBorder()
            + childSc.getTopMargin() + childSc.getBottomMargin();
    }

    for (uint32_t i = 0; i < childCount; i++)
    {
        auto& child = children[i]->getPayload()->node_;
        auto& childTc = child.transformContext;
        auto& childBTc = child.borderTransformContext;
        auto& childSc = child.styleContext;

        const int32_t bX = childSc.getRightBorder() + childSc.getLeftBorder();
        const int32_t bY = childSc.getTopBorder() + childSc.getBottomBorder();
        const int32_t bmX = bX + childSc.getRightMargin() + childSc.getLeftMargin();
        const int32_t bmY = bY + childSc.getTopMargin() + childSc.getBottomMargin();

        float parentSizeX = thisTc_->getContentScale().x - combinedPaddingX;
        float parentSizeY = thisTc_->getContentScale().y - combinedPaddingY;

        /* Compute child's content scale */
        childTc.setContentScale(
            {
                ceil(computeHorizontalScale(childSc.getHSizeConfig(), childCount, parentSizeX)),
                ceil(computeVerticalScale(childSc.getVSizeConfig(), childCount, parentSizeY))
            });

        /* Compute child's total scale (borders+margins) */
        childTc.setScale({ childTc.getContentScale().x + bmX, childTc.getContentScale().y + bmY });

        /* Set child's content border scale. We don't care about absolute size */
        childBTc.setContentScale({ childTc.getContentScale().x + bX, childTc.getContentScale().y + bY });

        /* How much we need to advance to place next child. Previous end + current element's total scale */
        nextXAdvance = startPosX + childTc.getScale().x;

        /* Take care of row wrapping if option is enabled */
        if (styleContextInj_->isRowWrappingEnabled())
        {
            /* This basically "spawns" a new row */
            if (nextXAdvance > parentSizeX)
            {
                startPosX = 0;
                startPosY += highestYOnRow;

                rowLastId = i;
                resolveRowChildrenAlignment(children, rowFirstId, rowLastId, highestYOnRow);
                rowFirstId = i;

                nextXAdvance = childTc.getScale().x;
                highestYOnRow = 0;
            }
        }

        /* Needed so Y of wrapped child starts at lowest Y possible */
        highestYOnRow = std::max(childTc.getScale().y, highestYOnRow);

        /* Set absolute position of element */
        childTc.setPos({ startPosX, startPosY });

        /* Set the start X to the next advance position*/
        startPosX = nextXAdvance;

        /* Set element's contentPos taking into account BM*/
        childTc.setContentPos(
            {
                childTc.getPos().x + childSc.getLeftMargin() + childSc.getLeftBorder(),
                childTc.getPos().y + childSc.getTopMargin() + childSc.getTopBorder()
            });

        /* Set element's border contentPos taking into account PB. We don't care about absolute pos */
        childBTc.setContentPos(
            {
                childTc.getContentPos().x - childSc.getLeftBorder(),
                childTc.getContentPos().y - childSc.getTopBorder()
            });
    }

    /* ------------------------------------- FINAL PUSH INTO PLACE ----------------------------------- */
    /* Align children based on their individual alignment option on the row they live in. This needs to be done
       in another pass because we need previous pass' information */
    resolveRowChildrenAlignment(children, rowFirstId, childCount, highestYOnRow);

    /* Above calculations have been with respect to 0,0 ,so now we need to offset the generated "container"
       by the container's parent alignment type.
       This needs to be done here because otherwise we can get wrong min max information and bottom/center alignments
       need this information. It's a trade-off. */
    pushElementsIntoPosition(children);
    /* ----------------------------------------------------------------------------------------------- */
}

void HkConstraintContext::resolveHorizontalPinch(HkTreeStruct& children)
{
    float startPosX = 0;
    float xSize = 0;
    float xAdvanceFraction = 0.0f;
    float advancePerc = 0.0f;

    const uint32_t childrenCount = children.size() - sbCount_;
    for (uint32_t i = 0; i < childrenCount; i++)
    {
        auto& child = children[i]->getPayload()->node_;
        auto& childTc = child.transformContext;
        auto& childSc = child.styleContext;

        auto sizeCfg = childSc.getHSizeConfig();
        auto pinchCfg = childSc.getPinchConfig();
        const float pScaleX = (float)thisTc_->getContentScale().x;
        const float minVal = sizeCfg.min / pScaleX;
        const float maxVal = sizeCfg.max / pScaleX;

        /* Verify if with the new offset, min/max val for THIS item will be exceeded.
           If yes, limit OFFSET so that limits are not exceeded */
        if (sizeCfg.value + pinchCfg.offsetX >= maxVal)
            pinchCfg.offsetX = maxVal - sizeCfg.value;
        if (sizeCfg.value + pinchCfg.offsetX <= minVal)
            pinchCfg.offsetX = minVal - sizeCfg.value;

        /* Verify constraints of the next item as well.
           Verify next item (it will change alongside this one) in roughly the same way.
           Difference here is that we calculate 'maybeOffsetForNext' as the maximum 'offset' the next item
           could support without exceeding limits.
        */
        float maybeOffsetForNext = pinchCfg.offsetX;
        if (i + 1 < childrenCount)
        {
            auto& childN = children[i + 1]->getPayload()->node_;
            auto& childNSc = childN.styleContext;
            auto sizeNCfg = childNSc.getHSizeConfig();

            const auto minValN = (sizeNCfg.min) / (float)pScaleX;
            if (sizeNCfg.value - pinchCfg.offsetX <= minValN)
            {
                maybeOffsetForNext = pinchCfg.offsetX - (minValN - (sizeNCfg.value - pinchCfg.offsetX));
            }
        }

        /* Items will change according to the minimum distance we can travel without breaking limits*/
        pinchCfg.offsetX = std::min(pinchCfg.offsetX, maybeOffsetForNext);

        /* Apply new offset and value*/
        sizeCfg.value += pinchCfg.offsetX;
        if (i + 1 < childrenCount)
        {
            auto& childN = children[i + 1]->getPayload()->node_;
            auto& childNSc = childN.styleContext;
            auto sizeNCfg = childNSc.getHSizeConfig();
            sizeNCfg.value -= pinchCfg.offsetX;
            childNSc.setHSizeConfig(sizeNCfg);
        }
        pinchCfg.offsetX = 0.0f;
        childSc.setPinchConfig(pinchCfg);
        childSc.setHSizeConfig(sizeCfg);

        /* Calculate position similat to grid layout*/
        startPosX = xAdvanceFraction * pScaleX;
        advancePerc += sizeCfg.value;
        xAdvanceFraction = advancePerc;
        childTc.setContentPos({ startPosX + (float)thisTc_->getContentPos().x , thisTc_->getContentPos().y });

        /* Scale elements according to their config. Subtract right margin to for the pinch zone*/
        xSize = childSc.getHSizeConfig().value * pScaleX;
        xSize -= (i == childrenCount - 1) ? 0 : pinchCfg.grabSize;

        /*Ceil is needed so we don't get off by one pixel artifacts*/
        childTc.setContentScale({ ceil(xSize), thisTc_->getContentScale().y });

        childTc.copyContentDataToAbsoluteData();
    }

    //NOTE: Possible hack for last item not covering all the space aka one stray column of pixels at the end
    // not covered.
    auto& child = children[childrenCount - 1]->getPayload()->node_;
    auto& childTc = child.transformContext;
    if (childTc.getContentPos().x + childTc.getContentScale().x < thisTc_->getContentPos().x + thisTc_->getContentScale().x)
    {
        const auto diff = thisTc_->getContentPos().x + thisTc_->getContentScale().x
            - (childTc.getContentPos().x + childTc.getContentScale().x);
        childTc.addContentScale({ diff, 0 });
        childTc.copyContentDataToAbsoluteData();
    }
}

void HkConstraintContext::resolveVerticalContainer(HkTreeStruct& children)
{
    int32_t startPosX = 0, startPosY = 0;
    int32_t longestXOnCol = 0;
    uint32_t colFirstId = 0;
    uint32_t colLastId = 0;
    int32_t nextYAdvance = 0;
    uint32_t childCount = children.size() - sbCount_;

    int32_t combinedPaddingX = 0;
    int32_t combinedPaddingY = 0;
    for (uint32_t i = 0; i < childCount; i++)
    {
        auto& child = children[i]->getPayload()->node_;
        auto& childSc = child.styleContext;

        combinedPaddingX += childSc.getRightBorder() + childSc.getLeftBorder()
            + childSc.getRightMargin() + childSc.getLeftMargin();
        combinedPaddingY += childSc.getTopBorder() + childSc.getBottomBorder()
            + childSc.getTopMargin() + childSc.getBottomMargin();
    }

    for (uint32_t i = 0; i < childCount; i++)
    {
        auto& child = children[i]->getPayload()->node_;
        auto& childTc = child.transformContext;
        auto& childBTc = child.borderTransformContext;
        auto& childSc = child.styleContext;

        const int32_t bX = childSc.getRightBorder() + childSc.getLeftBorder();
        const int32_t bY = childSc.getTopBorder() + childSc.getBottomBorder();
        const int32_t bmX = bX + childSc.getRightMargin() + childSc.getLeftMargin();
        const int32_t bmY = bY + childSc.getTopMargin() + childSc.getBottomMargin();

        float parentSizeX = thisTc_->getContentScale().x - combinedPaddingX;
        float parentSizeY = thisTc_->getContentScale().y - combinedPaddingY;

        /* Compute child's content scale */
        childTc.setContentScale(
            {
                ceil(computeHorizontalScale(childSc.getHSizeConfig(), childCount, parentSizeX)),
                ceil(computeVerticalScale(childSc.getVSizeConfig(), childCount, parentSizeY))
            });

        /* Compute child's total scale (borders+margins) */
        childTc.setScale({ childTc.getContentScale().x + bmX, childTc.getContentScale().y + bmY });

        /* Set child's content border scale. We don't care about absolute size */
        childBTc.setContentScale({ childTc.getContentScale().x + bX, childTc.getContentScale().y + bY });

        /* How much we need to advance to place next child. Previous end + current element's total scale */
        nextYAdvance = startPosY + childTc.getScale().y;

        /* Take care of col wrapping if option is enabled */
        if (styleContextInj_->isColWrappingEnabled())
        {
            /* This basically "spawns" a new col */
            if (nextYAdvance > parentSizeY)
            {
                startPosY = 0;
                startPosX += longestXOnCol;

                colLastId = i;
                resolveColChildrenAlignment(children, colFirstId, colLastId, longestXOnCol);
                colFirstId = i;

                nextYAdvance = childTc.getScale().y;
                longestXOnCol = 0;
            }
        }

        /* Needed so X of wrapped child starts at lowest X possible */
        longestXOnCol = std::max(childTc.getScale().x, longestXOnCol);

        /* Set absolute position of element */
        childTc.setPos({ startPosX, startPosY });

        /* Set the start Y to the next advance position*/
        startPosY = nextYAdvance;

        /* Set element's contentPos taking into account BM*/
        childTc.setContentPos(
            {
                childTc.getPos().x + childSc.getLeftMargin() + childSc.getLeftBorder(),
                childTc.getPos().y + childSc.getTopMargin() + childSc.getTopBorder()
            });

        /* Set element's border contentPos taking into account PB. We don't care about absolute pos */
        childBTc.setContentPos(
            {
                childTc.getContentPos().x - childSc.getLeftBorder(),
                childTc.getContentPos().y - childSc.getTopBorder()
            });
    }

    /* ------------------------------------- FINAL PUSH INTO PLACE ----------------------------------- */
    /* Align children based on their individual alignment option on the row they live in. This needs to be done
       in another pass because we need previous pass' information */
    resolveColChildrenAlignment(children, colFirstId, childCount, longestXOnCol);

    /* Above calculations have been with respect to 0,0 ,so now we need to offset the generated "container"
       by the container's parent alignment type.
       This needs to be done here because otherwise we can get wrong min max information and bottom/center alignments
       need this information. It's a trade-off. */
    pushElementsIntoPosition(children);
    /* ----------------------------------------------------------------------------------------------- */
}

void HkConstraintContext::resolveVerticalPinch(HkTreeStruct& children)
{
    float startPosY = 0;
    float ySize = 0;
    float yAdvanceFraction = 0.0f;
    float advancePerc = 0.0f;

    const uint32_t childrenCount = children.size() - sbCount_;
    for (uint32_t i = 0; i < childrenCount; i++)
    {
        auto& child = children[i]->getPayload()->node_;
        auto& childTc = child.transformContext;
        auto& childSc = child.styleContext;

        auto sizeCfg = childSc.getVSizeConfig();
        auto pinchCfg = childSc.getPinchConfig();
        const float pScaleY = (float)thisTc_->getContentScale().y;
        const float minVal = sizeCfg.min / pScaleY;
        const float maxVal = sizeCfg.max / pScaleY;

        /* Verify if with the new offset, min/max val for THIS item will be exceeded.
           If yes, limit OFFSET so that limits are not exceeded */
        if (sizeCfg.value + pinchCfg.offsetY >= maxVal)
            pinchCfg.offsetY = maxVal - sizeCfg.value;
        if (sizeCfg.value + pinchCfg.offsetY <= minVal)
            pinchCfg.offsetY = minVal - sizeCfg.value;

        /* Verify constraints of the next item as well.
           Verify next item (it will change alongside this one) in roughly the same way.
           Difference here is that we calculate 'maybeOffsetForNext' as the maximum 'offset' the next item
           could support without exceeding limits.
        */
        float maybeOffsetForNext = pinchCfg.offsetY;
        if (i + 1 < childrenCount)
        {
            auto& childN = children[i + 1]->getPayload()->node_;
            auto& childNSc = childN.styleContext;
            auto sizeNCfg = childNSc.getVSizeConfig();

            const auto minValN = (sizeNCfg.min) / (float)pScaleY;
            if (sizeNCfg.value - pinchCfg.offsetY <= minValN)
            {
                maybeOffsetForNext = pinchCfg.offsetY - (minValN - (sizeNCfg.value - pinchCfg.offsetY));
            }
        }

        /* Items will change according to the minimum distance we can travel without breaking limits*/
        pinchCfg.offsetY = std::min(pinchCfg.offsetY, maybeOffsetForNext);

        /* Apply new offset and value*/
        sizeCfg.value += pinchCfg.offsetY;
        if (i + 1 < childrenCount)
        {
            auto& childN = children[i + 1]->getPayload()->node_;
            auto& childNSc = childN.styleContext;
            auto sizeNCfg = childNSc.getVSizeConfig();
            sizeNCfg.value -= pinchCfg.offsetY;
            childNSc.setVSizeConfig(sizeNCfg);
        }
        pinchCfg.offsetY = 0.0f;
        childSc.setPinchConfig(pinchCfg);
        childSc.setVSizeConfig(sizeCfg);

        /* Calculate position similat to grid layout*/
        startPosY = yAdvanceFraction * pScaleY;
        advancePerc += sizeCfg.value;
        yAdvanceFraction = advancePerc;
        childTc.setContentPos({ thisTc_->getContentPos().x, startPosY + (float)thisTc_->getContentPos().y });

        /* Scale elements according to their config. Subtract right margin to for the pinch zone*/
        ySize = childSc.getVSizeConfig().value * pScaleY;
        ySize -= (i == childrenCount - 1) ? 0 : pinchCfg.grabSize;

        /*Ceil is needed so we don't get off by one pixel artifacts*/
        childTc.setContentScale({ thisTc_->getContentScale().x, ceil(ySize) });

        childTc.copyContentDataToAbsoluteData();
    }

    //NOTE: Possible hack for last item not covering all the space aka one stray column of pixels at the end
    // not covered.
    auto& child = children[childrenCount - 1]->getPayload()->node_;
    auto& childTc = child.transformContext;
    if (childTc.getContentPos().y + childTc.getContentScale().y < thisTc_->getContentPos().y + thisTc_->getContentScale().y)
    {
        const auto diff = thisTc_->getContentPos().y + thisTc_->getContentScale().y
            - (childTc.getContentPos().y + childTc.getContentScale().y);
        childTc.addContentScale({ 0, diff });
        childTc.copyContentDataToAbsoluteData();
    }
}

void HkConstraintContext::resolveRowChildrenAlignment(HkTreeStruct& children,
    const uint32_t rowFirstId, const uint32_t rowLastId, const uint32_t highestYOnRow) const
{
    for (uint32_t j = rowFirstId; j < rowLastId; j++)
    {
        auto& child = children[j]->getPayload()->node_;
        auto& childBpTc = child.transformContext;
        auto& childBpBTc = child.borderTransformContext;
        auto& childBpSc = child.styleContext;

        //TODO: This isnt the value we shall use. To be fixed by future feature
        switch (childBpSc.getVAlignment())
        {
        case HkVAlignment::Top:
            /*Valid, but do nothing */
            break;
        case HkVAlignment::Center:
            childBpTc.addPos({ 0, highestYOnRow / 2 - childBpTc.getScale().y / 2 });
            childBpTc.addContentPos({ 0, highestYOnRow / 2 - childBpTc.getScale().y / 2 });
            childBpBTc.addContentPos({ 0, highestYOnRow / 2 - childBpTc.getScale().y / 2 });
            break;
        case HkVAlignment::Bottom:
            childBpTc.addPos({ 0, highestYOnRow - childBpTc.getScale().y });
            childBpTc.addContentPos({ 0, highestYOnRow - childBpTc.getScale().y });
            childBpBTc.addContentPos({ 0, highestYOnRow - childBpTc.getScale().y });
            break;
        }
    }
}

void HkConstraintContext::resolveColChildrenAlignment(HkTreeStruct& children,
    const uint32_t colFirstId, const uint32_t colLastId, const uint32_t longestXOnCol) const
{
    for (uint32_t j = colFirstId; j < colLastId; j++)
    {
        auto& child = children[j]->getPayload()->node_;
        auto& childBpTc = child.transformContext;
        auto& childBpBTc = child.borderTransformContext;
        auto& childBpSc = child.styleContext;

        switch (childBpSc.getHAlignment())
        {
        case HkHAlignment::Left:
            /*Valid, but do nothing */
            break;
        case HkHAlignment::Center:
            childBpTc.addPos({ longestXOnCol / 2 - childBpTc.getScale().x / 2, 0 });
            childBpTc.addContentPos({ longestXOnCol / 2 - childBpTc.getScale().x / 2, 0 });
            childBpBTc.addContentPos({ longestXOnCol / 2 - childBpTc.getScale().x / 2, 0 });
            break;
        case HkHAlignment::Right:
            childBpTc.addPos({ longestXOnCol - childBpTc.getScale().x, 0 });
            childBpTc.addContentPos({ longestXOnCol - childBpTc.getScale().x, 0 });
            childBpBTc.addContentPos({ longestXOnCol - childBpTc.getScale().x, 0 });
            break;
        }
    }
}

void HkConstraintContext::pushElementsIntoPosition(HkTreeStruct& children) const
{
    int32_t startPosX = 0, startPosY = 0;
    MinMaxPos result = getMinAndMaxPositions(children); // we calculate the same thing later. optimize

    const int32_t lPadd = styleContextInj_->getLeftPadding();
    const int32_t rPadd = styleContextInj_->getRightPadding();
    const int32_t tPadd = styleContextInj_->getTopPadding();
    const int32_t bPadd = styleContextInj_->getBottomPadding();
    const glm::ivec2 cp = { thisTc_->getContentPos().x + lPadd, thisTc_->getContentPos().y + tPadd };
    const glm::ivec2 cs = { thisTc_->getContentScale().x - (lPadd + rPadd), thisTc_->getContentScale().y - (bPadd + tPadd) };

    /* Calculate start point for placement on the X axis based on container options*/
    switch (styleContextInj_->getHAlignment())
    {
    case HkHAlignment::Left:
        startPosX = cp.x;
        break;
    case HkHAlignment::Center:
        startPosX = cp.x + cs.x / 2 - ((result.maxX - result.minX) / 2);
        break;
    case HkHAlignment::Right:
        startPosX = (cp.x + cs.x) - (result.maxX - result.minX);
        break;
    }

    /* Calculate start point for placement on the X axis based on container options*/
    switch (styleContextInj_->getVAlignment())
    {
    case HkVAlignment::Top:
        startPosY = cp.y;
        break;
    case HkVAlignment::Center:
        startPosY = cp.y + cs.y / 2 - ((result.maxY - result.minY) / 2);
        break;
    case HkVAlignment::Bottom:
        startPosY = (cp.y + cs.y) - (result.maxY - result.minY);
        break;
    }

    /* Finnaly offset the elements */
    for (uint32_t i = 0; i < children.size() - sbCount_; i++)
    {
        auto& child = children[i]->getPayload()->node_;
        auto& childTc = child.transformContext;
        auto& childBTc = child.borderTransformContext;
        childTc.addPos({ startPosX , startPosY });
        childTc.addContentPos({ startPosX , startPosY });
        childBTc.addContentPos({ startPosX , startPosY });
    }
}

void HkConstraintContext::computeScrollBarCount()
{
    sbCount_ = 0;
    sbCount_ += (styleContextInj_->isOverflowAllowedX() && overflowXYSize_.x) ? 1 : 0;
    sbCount_ += (styleContextInj_->isOverflowAllowedY() && overflowXYSize_.y) ? 1 : 0;
}

/* This is only called on a container that has the possibility to have children*/
void HkConstraintContext::computeChildrenOverflowBasedOnMinMax(const MinMaxPos& minMax,
    const HkScrollbarsSize sbSizes)
{
    /*Reset variables */
    isOverflowX_ = false;
    isOverflowY_ = false;
    overflowXYSize_.x = 0;
    overflowXYSize_.y = 0;

    const int32_t lPadd = styleContextInj_->getLeftPadding();
    const int32_t rPadd = styleContextInj_->getRightPadding();
    const int32_t tPadd = styleContextInj_->getTopPadding();
    const int32_t bPadd = styleContextInj_->getBottomPadding();
    const glm::ivec2 cp = { thisTc_->getContentPos().x + lPadd, thisTc_->getContentPos().y + tPadd };
    const glm::ivec2 cs = { thisTc_->getContentScale().x - (lPadd + rPadd), thisTc_->getContentScale().y - (bPadd + tPadd) };

    /* Calculate X overflow */
    if (styleContextInj_->isOverflowAllowedX())
    {
        if (minMax.maxX > cp.x + cs.x)
        {
            isOverflowX_ = true;
            overflowXYSize_.x = minMax.maxX - (cp.x + cs.x);
        }

        if (minMax.minX < cp.x)
        {
            isOverflowX_ = true;
            overflowXYSize_.x += cp.x - minMax.minX;
        }

        /* Try to see if Y axis also needs to overflow to fit everything nicely*/
        if (isOverflowX_ && styleContextInj_->isOverflowAllowedY())
        {

            if (minMax.maxY + sbSizes.hsbSize > cp.y + cs.y)
            {
                isOverflowY_ = true;
                overflowXYSize_.y = sbSizes.hsbSize + minMax.maxY - (cp.y + cs.y);
                overflowXYSize_.x += sbSizes.vsbSize;
            }
            /* If there's been a calculation on Y already, exit prematurely*/
            return;
        }
    }

    /* Calculate Y overflow */
    if (styleContextInj_->isOverflowAllowedY())
    {
        if (minMax.maxY > cp.y + cs.y)
        {
            isOverflowY_ = true;
            overflowXYSize_.y = minMax.maxY - (cp.y + cs.y);
        }

        if (minMax.minY < cp.y)
        {
            isOverflowY_ = true;
            overflowXYSize_.y += cp.y - minMax.minY;
        }

        /* Try to see if X axis also needs to overflow to fit everything nicely*/
        if (isOverflowY_ && styleContextInj_->isOverflowAllowedX())
        {
            if (minMax.maxX + sbSizes.vsbSize > cp.x + cs.x)
            {
                isOverflowX_ = true;
                overflowXYSize_.x = sbSizes.vsbSize + minMax.maxX - (cp.x + cs.x);
                overflowXYSize_.y += sbSizes.hsbSize;
            }
            /* If there's been a calculation on Y already, exit prematurely*/
            return;
        }
    }
}

float HkConstraintContext::computeHorizontalScale(const HkSizeConfig& config, const uint32_t childCount, const float parentSize)
{
    /* When it comes to parent relative scaling, use the "contentScale" instead of just "scale", we don't
       need the absolute scale here */
    float size = 1.0f;
    switch (config.type)
    {
    case HkSizeType::Absolute:
        size = config.value;
        break;
    case HkSizeType::PercParent:
        size = config.value * parentSize;
        break;
    case HkSizeType::FitParent:
        size = parentSize;
        break;
    case HkSizeType::Balanced:
        size = parentSize / childCount;
        break;
    case HkSizeType::FitCell:
    case HkSizeType::PercCell:
        /* Fall through, unsupported mode by horizontal/vertical layout*/
        break;
    }
    return std::clamp(size, config.min, config.max);
}

float HkConstraintContext::computeVerticalScale(const HkSizeConfig& config, const uint32_t childCount, const float parentSize)
{
    /* When it comes to parent relative scaling, use the "contentScale" instead of just "scale", we don't
       need the absolute scale here */
    float size = 1.0f;
    switch (config.type)
    {
    case HkSizeType::Absolute:
        size = config.value;
        break;
    case HkSizeType::PercParent:
        size = config.value * parentSize;
        break;
    case HkSizeType::FitParent:
        size = parentSize;
        break;
    case HkSizeType::Balanced:
        size = parentSize / childCount;
        break;
    case HkSizeType::FitCell:
    case HkSizeType::PercCell:
        /* Fall through, unsupported mode by horizontal/vertical layout*/
        break;
    }

    return std::clamp(size, config.min, config.max);
}

void HkConstraintContext::resolveAxisOverflow(const HkTreeStruct& children,
    const HkScrollbarsSize sbSizes)
{
    MinMaxPos result = getMinAndMaxPositions(children);
    computeChildrenOverflowBasedOnMinMax(result, sbSizes);
    for (uint32_t i = 0; i < children.size() - sbCount_; i++)
    {
        auto& child = children[i]->getPayload()->node_;
        auto& childTc = child.transformContext;
        auto& childBTc = child.borderTransformContext;
        const auto leftOverflow = (result.minX < thisTc_->getContentPos().x) ? thisTc_->getContentPos().x - result.minX : 0;
        const auto topOverflow = (result.minY < thisTc_->getContentPos().y) ? thisTc_->getContentPos().y - result.minY : 0;
        const auto offsetXY = glm::ivec2(leftOverflow + offsetPercentage_.x * -overflowXYSize_.x,
            topOverflow + offsetPercentage_.y * -overflowXYSize_.y);
        childTc.addPos(offsetXY);
        childTc.addContentPos(offsetXY);
        childBTc.addContentPos(offsetXY);
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
    /* setting scale of knob */
    const auto preKNobSize = std::min(thisTc_->getContentScale().x, thisTc_->getContentScale().y);
    knobTc.setContentScale({ preKNobSize, preKNobSize });

    const auto margins = 2;
    const auto minAxisKobSize = 30;
    /* Orientation calcs do differ */
    if (isFromHorizontalSB)
    {
        const auto knobSizeX = thisTc_->getContentScale().x - overflowSize;
        knobTc.setContentScale({ std::max(minAxisKobSize, knobSizeX) - 2 * margins , preKNobSize - 2 * margins });
        const auto minX = thisTc_->getContentPos().x + margins;
        const auto maxX = thisTc_->getContentPos().x + thisTc_->getContentScale().x - knobTc.getContentScale().x - margins;
        const auto posX = minX * (1.0f - currKnobValue) + currKnobValue * maxX;
        knobTc.setContentPos({ posX, thisTc_->getContentPos().y + margins });
    }
    else
    {
        const auto knobSizeY = thisTc_->getContentScale().y - overflowSize;
        knobTc.setContentScale({ preKNobSize - 2 * margins, std::max(minAxisKobSize, knobSizeY) - 2 * margins });
        const auto minY = thisTc_->getContentPos().y + margins;
        const auto maxY = thisTc_->getContentPos().y + thisTc_->getContentScale().y - knobTc.getContentScale().y - margins;
        const auto posY = minY * (1.0f - currKnobValue) + currKnobValue * maxY;
        knobTc.setContentPos({ thisTc_->getContentPos().x + margins, posY });
    }

    knobTc.copyContentDataToAbsoluteData();
}

/* Keeps scrollbar object at the bottom or at the right of the container. */
void HkConstraintContext::scrollBarConstrain(HkTransformContext& scrollBarTc, const uint32_t otherSbMargin) const
{
    ScrollbarMargin margins;
    const auto sbScale = scrollBarTc.getContentScale();
    (sbScale.x > sbScale.y) ? (margins.hsbMargin = sbScale.y) : (margins.vsbMargin = sbScale.x);

    const auto barScale = std::max(margins.hsbMargin, margins.vsbMargin);
    if (margins.hsbMargin && isOverflowX_)
    {
        /* We should take into account if vertical bar is present so that bottom right 'intersection'
           between bars is filled or not. Same thing should apply for vertical calcs bellow */
        const auto verticalBarAwareMargin = isOverflowY_ ? otherSbMargin : 0;
        scrollBarTc.setContentScale({ thisTc_->getContentScale().x - verticalBarAwareMargin, barScale });
        scrollBarTc.setContentPos({ thisTc_->getContentPos().x, thisTc_->getContentPos().y + thisTc_->getContentScale().y - barScale });
    }
    else if (margins.vsbMargin && isOverflowY_)
    {
        const auto horizontalBarAwareMargin = isOverflowX_ ? otherSbMargin : 0;
        scrollBarTc.setContentScale({ barScale, thisTc_->getContentScale().y - horizontalBarAwareMargin });
        scrollBarTc.setContentPos({ thisTc_->getContentPos().x + thisTc_->getContentScale().x - barScale, thisTc_->getContentPos().y });
    }
    scrollBarTc.copyContentDataToAbsoluteData();
}

/* WindowFrame is a special UI element that 'drags' a container along with it that sits underneath the window frame. This
   function helps constraint that container to the windowFrame element */
void HkConstraintContext::windowFrameContainerConstraint(HkTransformContext& titleLabel, HkTransformContext& wfCtr,
    HkTransformContext& wfBorder, HkTransformContext& exitBtn, HkTransformContext& minBtn) const
{
    const int32_t buttonScale = 20;
    /* TODO: dirty flags shall be used here to not do redundant repositioning */
    wfCtr.setContentPos({ thisTc_->getContentPos().x, thisTc_->getContentPos().y + thisTc_->getContentScale().y });
    wfCtr.setContentScale({ thisTc_->getContentScale().x, wfCtr.getContentScale().y });

    titleLabel.setContentScale({ 200 , thisTc_->getContentScale().y });
    titleLabel.setContentPos({ thisTc_->getContentPos().x + 5, thisTc_->getContentPos().y });

    exitBtn.setContentScale({ buttonScale, buttonScale }); // hardcoded, but technically ok situation. Edit: fix later
    exitBtn.setContentPos({
        thisTc_->getContentPos().x + thisTc_->getContentScale().x - buttonScale - buttonScale / 2,
        thisTc_->getContentPos().y + buttonScale / 2 - 5
        });

    minBtn.setContentScale({ buttonScale, buttonScale });
    minBtn.setContentPos({
        thisTc_->getContentPos().x + thisTc_->getContentScale().x - buttonScale - buttonScale * 2,
        thisTc_->getContentPos().y + buttonScale / 2 - 5
        });

    //TODO: Maybe not a good ideea to do it here
    // wfBorder.setContentScale({ wfCtr.getContentScale().x + 20,  wfCtr.getContentScale().y + 10 });
    // wfBorder.setContentPos({ wfCtr.getContentPos().x - 10, wfCtr.getContentPos().y });

    wfCtr.copyContentDataToAbsoluteData();
    titleLabel.copyContentDataToAbsoluteData();
    exitBtn.copyContentDataToAbsoluteData();
    minBtn.copyContentDataToAbsoluteData();
}

void HkConstraintContext::windowFrameContainerConstraint(HkTransformContext& titleLabel, HkTransformContext& wfCtr,
    HkTransformContext& wfBorder, HkTransformContext& exitBtn, HkTransformContext& minBtn, const glm::ivec2& windowSize,
    const bool isFullscreen) const
{
    if (!isFullscreen)
    {
        windowFrameContainerConstraint(titleLabel, wfCtr, wfBorder, exitBtn, minBtn);
        return;
    }

    thisTc_->setContentPos({ 0,0 });
    /* Necessary so we still "render" the frame in renderMySelf scissor pass */
    thisTc_->setContentScale({ -1,-1 });

    // wfCtr.setScale(windowSize + 1); //TODO: hacK: looks like wfCtr gets a windowSize thats lacking exactly 1px behind (looks like GPU dependent)
    wfCtr.setContentScale(windowSize);
    wfCtr.setContentPos({ 0,0 });

    wfCtr.copyContentDataToAbsoluteData();
    thisTc_->copyContentDataToAbsoluteData();
}

void HkConstraintContext::injectStyleContext(HkStyleContext* styleContext)
{
    styleContextInj_ = styleContext;
}
} // hkui