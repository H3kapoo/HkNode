#include "HkScrollBar.hpp"

namespace hkui
{
HkScrollBar::HkScrollBar(const std::string& name, const bool isHorizontal)
    : HkNodeBase(name, "ScrollBar")
    , knob_("{Internal}-KnobFor " + name, isHorizontal)
    , overflowSize_{ 0 }
    , isHorizontal_(isHorizontal)
    , isActive_{ false }
{
    node_.renderContext.setShaderSource("assets/shaders/v1.glsl", "assets/shaders/f1.glsl");
    node_.renderContext.getShader().setVec3f("color", glm::vec3(0.2f, 1.0f, 0.2f)); // gray
    node_.renderContext.render(sceneDataRef_.sceneProjMatrix, node_.transformContext.getModelMatrix());

    treeStruct_.pushChild(&knob_.treeStruct_);
    setBarScale(20); /* default bar scale depending on orientation */
}

void HkScrollBar::onDrag()
{
    knob_.computeKnobValue({ -knob_.node_.transformContext.getScale().x / 2,
            -knob_.node_.transformContext.getScale().y / 2 });
}

void HkScrollBar::onClick()
{
    knob_.computeKnobValue({ -knob_.node_.transformContext.getScale().x / 2,
           -knob_.node_.transformContext.getScale().y / 2 });
}

void HkScrollBar::resolveChildrenConstraints(std::vector<HkTreeStructure<HkNodeBase>*>&)
{
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