#pragma once

#include <vector>
#include <numeric>

#include "../base/HkTreeStructure.hpp"
#include "HkTransformContext.hpp"

namespace hkui
{
enum class HkConstraintPolicy
{
    AlignLeftToRight,
    AlignTopToBottom,
    AlignEvenLeftToRight,
    AlignCenterLeftToRight
};

enum class HkChildrenOrientation
{
    Horizontal,
    Vertical
};

struct MaxAndTotal
{
    int max{ 0 };
    int total{ 0 };
};

struct MinMaxPos
{
    int min{ 0 };
    int max{ 0 };
};

struct ScrollbarMargin
{
    uint32_t hsbMargin{ 0 };
    uint32_t vsbMargin{ 0 };
};

struct HkStyleParams
{
    /* Element's margins */
    uint32_t marginLX{ 0 }, marginRX{ 0 };
    uint32_t marginLY{ 0 }, marginRY{ 0 };
};

class HkNodeBase;

class HkConstraintContext
{
public:
    HkConstraintContext() : isOverflowAllowedX_{ true }, isOverflowAllowedY_{ true }
        , isOverflowX_{ false }, isOverflowY_{ false }
        , overflowXYSize_{ 0,0 }, offsetPercentage_{ 0,0 }, sbCount_{ 0 }
        , policy_{ HkConstraintPolicy::AlignLeftToRight } {}

    void setRootTc(HkTransformContext* rootTc);
    void setPolicy(const HkConstraintPolicy policy);
    void resolveConstraints(std::vector<HkTreeStructure<HkNodeBase>*>& children);

    /* Computes */
    void computeScrollBarCount();
    void computeOverflowBasedOnMinMax(const MinMaxPos& minMax);

    MinMaxPos getMinAndMaxPositions(const std::vector<HkTreeStructure<HkNodeBase>*>& children);
    ScrollbarMargin getScrollbarMargins(const std::vector<HkTreeStructure<HkNodeBase>*>& children) const;
    MaxAndTotal getVerticalMaxValueAndTotalHeightValue(const std::vector<HkTreeStructure<HkNodeBase>*>& children);
    MaxAndTotal getHorizontalMaxValueAndTotalWidthValue(const std::vector<HkTreeStructure<HkNodeBase>*>& children);

    void resolveChildrenOverflowVariables(const HkChildrenOrientation orientation,
        const std::vector<HkTreeStructure<HkNodeBase>*>& children);

    void alignTopBottom(const std::vector<HkTreeStructure<HkNodeBase>*>& children);
    void alignCenterLeftRight(const std::vector<HkTreeStructure<HkNodeBase>*>& children);
    void alignEvenLeftRight(const std::vector<HkTreeStructure<HkNodeBase>*>& children);
    void alignLeftRight(const std::vector<HkTreeStructure<HkNodeBase>*>& children);

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

private:
    HkStyleParams styleParams_;
    HkConstraintPolicy policy_;
    HkTransformContext* thisTc_;
};
} // hkui