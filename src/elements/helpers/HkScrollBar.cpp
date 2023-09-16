#include "HkScrollBar.hpp"

namespace hkui
{
HkScrollBar::HkScrollBar(const std::string& name, const bool isHorizontal)
    : HkNodeBase(name, HkNodeType::ScrollBar)
    , knob_("{Internal}-KnobFor " + name, isHorizontal)
    , overflowSize_{ 0 }
    , isHorizontal_(isHorizontal)
    , isActive_{ false }
{
    node_.styleContext.setColor(glm::vec3(0.95f, 1.0f, 0.95f));
    node_.renderContext.setShaderSource("assets/shaders/v1.glsl", "assets/shaders/f1.glsl");

    treeStruct_.pushChild(&knob_.treeStruct_);
    setBarScale(20); /* default bar scale depending on orientation */
}

void HkScrollBar::onScroll()
{
    knob_.scrollOngoing();
}

void HkScrollBar::onDrag()
{
    /* Parameters represent offsets from drag or click area so that mouse doesnt snap to center
       of object and instead keeps a natural distance from its center */
    knob_.computeKnobValue({ -knob_.node_.transformContext.getScale().x / 2,
            -knob_.node_.transformContext.getScale().y / 2 });
}

void HkScrollBar::onClick()
{
    knob_.computeKnobValue({ -knob_.node_.transformContext.getScale().x / 2,
           -knob_.node_.transformContext.getScale().y / 2 });
}

void HkScrollBar::resolveChildrenConstraints(HkTreeStruct&, const HkScrollbarsSize&)
{
    //TODO: Add modifiers such as: min knob size and overflow size impact on knob scale factor
    node_.constraintContext.constrainSBKnob(isHorizontal_, overflowSize_, knob_.getValue(), knob_.node_.transformContext);
}

void HkScrollBar::setScrollValue(float value)
{
    knob_.setValue(value);
}

/* Scale here refers to when a container has an overflow, how big of an outset this scrollbar will be. */
void HkScrollBar::setBarScale(uint32_t scale)
{
    //TODO: If scale is not an even number, we might have to inevitably do with off by one visual errors,
    //      and in that case maybe it would be better to round to nearest even number.
    if (isHorizontal_)
    {
        node_.transformContext.setScale({ node_.transformContext.getScale().x, scale });
    }
    else
    {
        node_.transformContext.setScale({ scale, node_.transformContext.getScale().y });
    }
}

void HkScrollBar::setScrollBarActive(const bool isActive)
{
    isActive_ = isActive;
    /* Reset knob value on scrollbar deactivation */
    // if (!isActive_)
    // {
    //     setScrollValue(0);
    // }
}

void HkScrollBar::setOverflowSize(int value)
{
    if (value < 0) overflowSize_ = 0;
    overflowSize_ = value;
}

bool HkScrollBar::isHorizontalScrollBar() const
{
    return isHorizontal_;
}

float HkScrollBar::getScrollValue() const
{
    return knob_.getValue();
}

bool HkScrollBar::isScrollBarActive() const
{
    return isActive_;
}
} // hkui