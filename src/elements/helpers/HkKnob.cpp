#include "HkKnob.hpp"

namespace hkui
{
HkKnob::HkKnob(const std::string& name, const bool isHorizontal)
    : HkNodeBase(name, "Knob")
    , value_{ 0 }
    , isHorizontalKnob_(isHorizontal)
{
    node_.renderContext.setShaderSource("assets/shaders/v1.glsl", "assets/shaders/f1.glsl");
    node_.renderContext.shader.setVec3f("color", glm::vec3(0.5f, 0.7f, 1.0f)); // gray
    node_.renderContext.render(sceneDataRef_.sceneProjMatrix, node_.transformContext.getModelMatrix());
}

void HkKnob::onGeneralMouseMove()
{
    if (sceneDataRef_.isMouseClicked && sceneDataRef_.focusedId == treeStruct_.getId())
    {
        computeKnobValue();
    }
}

void HkKnob::computeKnobValue()
{
    auto& parentTc = treeStruct_.getParent()->getPayload()->node_.transformContext;
    if (isHorizontalKnob_)
    {
        const float minX = parentTc.pos.x - parentTc.scale.x / 2;
        const float maxX = parentTc.pos.x + parentTc.scale.x / 2;
        // https://rosettacode.org/wiki/Map_range
        setValue((sceneDataRef_.mousePos.x - minX) / (maxX - minX));
    }
    else
    {
        const float minY = parentTc.pos.y - parentTc.scale.y / 2;
        const float maxY = parentTc.pos.y + parentTc.scale.y / 2;
        // https://rosettacode.org/wiki/Map_range
        setValue((sceneDataRef_.mousePos.y - minY) / (maxY - minY));
    }
}

float HkKnob::getValue() const
{
    return value_;
}

bool HkKnob::isHorizontal() const
{
    return isHorizontalKnob_;
}

void HkKnob::setValue(float value)
{
    value_ = value;
    if (value_ < 0.0f) value_ = 0.0f;
    if (value_ > 1.0f) value_ = 1.0f;
}
} // hkui