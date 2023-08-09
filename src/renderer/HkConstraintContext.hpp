#pragma once

#include <vector>
#include <numeric>

#include "../base/HkTreeStructure.hpp"
#include "HkTransformContext.hpp"

namespace hkui
{
enum class HkConstraintPolicy
{
    AlignHorizontally,
};

class HkNodeBase;

class HkConstraintContext
{
public:
    HkConstraintContext() : isOverflowX{ false }, isOverflowY{ false } {}

    void setRootTc(HkTransformContext* rootTc);

    void setPolicy(const HkConstraintPolicy policy);

    void resolveConstraints(std::vector<HkTreeStructure<HkNodeBase>*>& children);

    void windowFrameContainerConstraint(HkTransformContext& childTc);

    void alignHorizontally(const std::vector<HkTreeStructure<HkNodeBase>*>& children);
    void alignVertically(const std::vector<HkTreeStructure<HkNodeBase>*>& children);

    /* Scrollbar related */
    void scrollBarConstrain(HkTransformContext& scrollBarTc, bool isHorizontalBar);
    void constrainSBKnob(bool isFromHorizontalSB, float currKnobValue, HkTransformContext& knobTc);

    void freeConstraint(const std::vector<HkTreeStructure<HkNodeBase>*>& children);

    // ConstraintParams..
    bool isOverflowX;
    bool isOverflowY;

    glm::ivec2 additionalOffset_{ 0,0 };
    uint32_t vScrollBarHeight_;
    uint32_t hScrollBarWidth_;

private:
    HkConstraintPolicy policy_;
    HkTransformContext* thisTc_;
};
} // hkui