#pragma once

#include <vector>
#include <numeric>

#include "../base/HkTreeStructure.hpp"
#include "HkTransformContext.hpp"
#include "HkStyleContext.hpp"

namespace hkui
{
enum class HkConstraintPolicy
{
    AlignEvenLeftToRight,
    AlignCenterLeftToRight,
    AlignLeftToRight,
    AlignEvenTopToBottom,
    AlignTopToBottom,
};


struct MaxAndTotal
{
    int max{ 0 };
    int total{ 0 };
};

struct MinMaxPos
{
    int32_t maxX{ -99999 }, maxY{ -99999 };
    int32_t minX{ 99999 }, minY{ 99999 };
};

struct MinMaxScale
{
    int32_t scaleX{ 0 }, scaleY{ 0 };
};

struct ScrollbarMargin
{
    uint32_t hsbMargin{ 0 };
    uint32_t vsbMargin{ 0 };
};

struct HkScrollbarsSize
{
    int32_t hsbSize{ 10 };
    int32_t vsbSize{ 10 };
};

class HkNodeData;
class HkNodeBase;
enum class HkNodeType;
using HkTreeStruct = std::vector<HkTreeStructure<HkNodeBase, HkNodeType>*>;

class HkConstraintContext
{
public:
    HkConstraintContext() : isOverflowX_{ false }, isOverflowY_{ false }
        , overflowXYSize_{ 0,0 }, offsetPercentage_{ 0,0 }, sbCount_{ 0 }
    {}

    void resolveConstraints(HkTreeStruct& children, const HkScrollbarsSize sbSizes);
    void resolveHorizontalContainer(HkTreeStruct& children);
    void resolveHorizontalPinch(HkTreeStruct& children);

    void resolveVerticalContainer(HkTreeStruct& children);
    void resolveVerticalPinch(HkTreeStruct& children);

    void resolveGridContainer(HkTreeStruct& children);
    void resolveAxisOverflow(const HkTreeStruct& children, const HkScrollbarsSize sbSizes);

    void resolveRowChildrenAlignment(HkTreeStruct& children, const uint32_t rowFirstId,
        const uint32_t rowLastId, const uint32_t highestYOnRow) const;
    void resolveColChildrenAlignment(HkTreeStruct& children, const uint32_t colFirstId,
        const uint32_t colLastId, const uint32_t longestXOnCol) const;
    void pushElementsIntoPosition(HkTreeStruct& children) const;

    void computeScrollBarCount();
    void computeChildrenOverflowBasedOnMinMax(const MinMaxPos& minMax,
        const HkScrollbarsSize sbSizes);
    float computeHorizontalScale(const HkSizeConfig& config, const uint32_t childCount);
    float computeVerticalScale(const HkSizeConfig& config, const uint32_t childCount);

    MinMaxPos getMinAndMaxPositions(const HkTreeStruct& children) const;

    /* Scrollbar related */
    void scrollBarConstrain(HkTransformContext& scrollBarTc, const uint32_t otherSbMargin) const;
    void constrainSBKnob(bool isFromHorizontalSB, int overflowSize, float currKnobValue,
        HkTransformContext& knobTc) const;

    /* WindowFrame related */
    void windowFrameContainerConstraint(HkTransformContext& titleLabel, HkTransformContext& wfCtr, HkTransformContext& wfBorder,
        HkTransformContext& exitBtn, HkTransformContext& minBtn) const;
    void windowFrameContainerConstraint(HkTransformContext& titleLabel, HkTransformContext& wfCtr, HkTransformContext& wfBorder,
        HkTransformContext& exitBtn, HkTransformContext& minBtn, const glm::ivec2& windowSize, const bool isFullscreen) const;

    /* Injects */
    void injectStyleContext(HkStyleContext* styleContext);
    void injectTransformContext(HkTransformContext* styleContext);

    /* Scrollbar related */
    bool isOverflowX_;
    bool isOverflowY_;
    glm::ivec2 overflowXYSize_;
    glm::vec2 offsetPercentage_;
    uint32_t sbCount_;

private:
    HkStyleContext* styleContextInj_;
    HkTransformContext* thisTc_;
};
} // hkui