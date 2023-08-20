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
    AlignTopToBottom
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

class HkNodeBase;

class HkConstraintContext
{

    //TODO: Each class shall have their global params default initted inside ctor
public:
    HkConstraintContext() : isOverflowX_{ false }, isOverflowY_{ false }
        , overflowXYSize_{ 0,0 }, offsetPercentage_{ 0,0 }
        , policy_{ HkConstraintPolicy::AlignLeftToRight } {}

    void setRootTc(HkTransformContext* rootTc);

    void setPolicy(const HkConstraintPolicy policy);

    void resolveConstraints(std::vector<HkTreeStructure<HkNodeBase>*>& children);

    MaxAndTotal getVerticalMaxValueAndTotalHeightValue(const std::vector<HkTreeStructure<HkNodeBase>*>& children);
    MaxAndTotal getHorizontalMaxValueAndTotalWidthValue(const std::vector<HkTreeStructure<HkNodeBase>*>& children);

    void resolveChildrenOverflowVariables(const HkChildrenOrientation orientation,
        const std::vector<HkTreeStructure<HkNodeBase>*>& children);

    void windowFrameContainerConstraint(HkTransformContext& childTc);

    void alignLeftRight(const std::vector<HkTreeStructure<HkNodeBase>*>& children);
    void alignTopBottom(const std::vector<HkTreeStructure<HkNodeBase>*>& children);

    void alignHorizontally(const std::vector<HkTreeStructure<HkNodeBase>*>& children);
    void alignVertically(const std::vector<HkTreeStructure<HkNodeBase>*>& children);

    /* Scrollbar related */
    void scrollBarConstrain(HkTransformContext& scrollBarTc, bool isHorizontalBar);
    void constrainSBKnob(bool isFromHorizontalSB, int overflowSize, float currKnobValue, HkTransformContext& knobTc);

    void freeConstraint(const std::vector<HkTreeStructure<HkNodeBase>*>& children);

    // ConstraintParams..
    bool isOverflowX_;
    bool isOverflowY_;

    glm::ivec2 overflowXYSize_;
    glm::vec2 offsetPercentage_;

private:
    HkConstraintPolicy policy_;
    HkTransformContext* thisTc_;
};
} // hkui