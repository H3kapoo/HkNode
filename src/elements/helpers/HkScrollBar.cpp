#include "HkScrollBar.hpp"

namespace hkui
{
HkScrollBar::HkScrollBar(const std::string& name, const bool isHorizontal)
    : HkNodeBase(name, "ScrollBar")
    , knob_("{Internal}-KnobFor " + name, isHorizontal)
    , isHorizontal_(isHorizontal)
{
    node_.renderContext.setShaderSource("assets/shaders/v1.glsl", "assets/shaders/f1.glsl");
    node_.renderContext.shader.setVec3f("color", glm::vec3(0.2f, 1.0f, 0.2f)); // gray
    node_.renderContext.render(sceneDataRef_.sceneProjMatrix, node_.transformContext.getModelMatrix());

    treeStruct_.pushChild(&knob_.treeStruct_);
}

void HkScrollBar::onGeneralMouseMove()
{
    if (sceneDataRef_.isMouseClicked && sceneDataRef_.focusedId == treeStruct_.getId())
    {
        knob_.computeKnobValue();
    }
}

void HkScrollBar::onGeneralMouseClick()
{
    if (sceneDataRef_.focusedId == treeStruct_.getId())
    {
        knob_.computeKnobValue();
    }
}

float HkScrollBar::getScrollValue() const
{
    return knob_.getValue();
}

bool HkScrollBar::isScrollBarActive() const
{
    return isActive_;
}

void HkScrollBar::setScrollBarActive(const bool isActive)
{
    isActive_ = isActive;
    /* Reset knob value on scrollbar deactivation */
    if (!isActive_)
    {
        setScrollValue(0);
    }
}

void HkScrollBar::setScrollValue(float value)
{
    knob_.setValue(value);
}

void HkScrollBar::resolveConstraints(std::vector<HkTreeStructure<HkNodeBase>*>&)
{
    node_.constraintContext.constrainSBKnob(isHorizontal_, knob_.getValue(), knob_.node_.transformContext);
}

bool HkScrollBar::isHorizontalScrollBar() const
{
    return isHorizontal_;
}

} // hkui