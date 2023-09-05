#pragma once

#include <vector>
#include <numeric>

#include "../base/HkTreeStructure.hpp"
#include "HkTransformContext.hpp"

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

enum class HkDirection
{
    Horizontal,
    Vertical
};

enum class HkAlignment
{
    Top,
    Bottom,
    Left,
    Center,
    Right,
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

struct HkStyleParams
{
    /* Element's margins */
    uint32_t marginLX{ 0 }, marginRX{ 0 };
    uint32_t marginTY{ 0 }, marginBY{ 0 };
};

class HkNodeBase;
enum class HkNodeType;
using HkTreeStruct = std::vector<HkTreeStructure<HkNodeBase, HkNodeType>*>;

class HkConstraintContext
{
public:
    HkConstraintContext() : isOverflowAllowedX_{ true }, isOverflowAllowedY_{ true }
        , isOverflowX_{ false }, isOverflowY_{ false }
        , overflowXYSize_{ 0,0 }, offsetPercentage_{ 0,0 }, sbCount_{ 0 }
        , policy_{ HkConstraintPolicy::AlignLeftToRight } {}


    void setDirection(HkDirection dir);

    void setVAlignment(HkAlignment alignment);

    void setHAlignment(HkAlignment alignment);

    void setRootTc(HkTransformContext* rootTc);
    void setPolicy(const HkConstraintPolicy policy);
    void resolveConstraints(HkTreeStruct& children, const HkScrollbarsSize sbSizes);
    void resolveHorizontalContainer(HkTreeStruct& children, const HkScrollbarsSize sbSizes);
    void resolveVerticalContainer(HkTreeStruct& children, const HkScrollbarsSize sbSizes);

    void backPropagateRowChange(HkTreeStruct& children, const uint32_t nextRowFirstId,
        const uint32_t lastRowEndId, const uint32_t highestYOnRow);
    void backPropagateColChange(HkTreeStruct& children, const uint32_t nextColFirstId,
        const uint32_t lastColEndId, const uint32_t longestXOnCol);

    void applyFinalOffsets(HkTreeStruct& children);

    /* Computes */
    void computeScrollBarCount();
    void computeChildrenOverflowBasedOnMinMax(const MinMaxPos& minMax,
        const HkScrollbarsSize sbSizes);

    /* Solvers */
    void resolveAxisOverflow(const HkTreeStruct& children, const HkScrollbarsSize sbSizes);

    MinMaxPos getMinAndMaxPositions(const HkTreeStruct& children);

    void alignEvenTopToBottom(const HkTreeStruct& children);
    void alignCenterLeftRight(const HkTreeStruct& children);
    void alignEvenLeftRight(const HkTreeStruct& children);

    /* Scrollbar related */
    void scrollBarConstrain(HkTransformContext& scrollBarTc);
    void constrainSBKnob(bool isFromHorizontalSB, int overflowSize, float currKnobValue, HkTransformContext& knobTc);

    /* WindowFrame related */
    void windowFrameContainerConstraint(HkTransformContext& wfCtr, HkTransformContext& exitBtn,
        HkTransformContext& minBtn) const;
    void windowFrameContainerConstraint(HkTransformContext& wfCtr, HkTransformContext& exitBtn,
        HkTransformContext& minBtn, const glm::ivec2& windowSize, const bool isFullscreen) const;

    /* Style related */
    const HkStyleParams& getStyle() const;

    // ConstraintParams..
    /* Scrollbar related */
    bool isOverflowAllowedX_;
    bool isOverflowAllowedY_;
    bool isOverflowX_;
    bool isOverflowY_;
    glm::ivec2 overflowXYSize_;
    glm::vec2 offsetPercentage_;

    uint32_t sbCount_;

    HkStyleParams styleParams_; //TODOl Shall be its own context
private:
    HkConstraintPolicy policy_;
    HkTransformContext* thisTc_;
    // Container related
    HkDirection direction_{ HkDirection::Horizontal };
    HkAlignment verticalAlignment_{ HkAlignment::Top };
    HkAlignment horizontalAlignment_{ HkAlignment::Left };
    //TODO: Bottom + RIght => permanent scrollbars??
};
} // hkui