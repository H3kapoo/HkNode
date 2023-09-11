#include "HkKnob.hpp"

namespace hkui
{
HkKnob::HkKnob(const std::string& name, const bool isHorizontal)
    : HkNodeBase(name, HkNodeType::Knob)
    , value_{ 0 }
    , isHorizontalKnob_(isHorizontal)
    , scrollSensitivity_{ 0.05f }
{
    node_.styleContext.setColor(glm::vec3(0.5f, 0.7f, 0.9f));
    node_.renderContext.setShaderSource("assets/shaders/v1.glsl", "assets/shaders/f1.glsl");
    node_.renderContext.render(sceneDataRef_.sceneProjMatrix, node_.transformContext.getModelMatrix());
}

void HkKnob::onDrag()
{
    computeKnobValue(sceneDataRef_.mouseOffsetFromFocusedCenter);
}

void HkKnob::onScroll()
{
    scrollOngoing();
}

void HkKnob::scrollOngoing()
{
    setValue(getValue() - sceneDataRef_.scrollPosY * scrollSensitivity_);
}

/* Based on where the mouse is at drag time, compute 0 to 1 range mapped from scrollbar min pos to
   scrollbar max pos */
void HkKnob::computeKnobValue(const glm::ivec2 offsetFromCenter)
{
    // Todo: removed parent caching because of crash happening when inside ctr is of size 580 and parent of size 600
    const auto& parentTc = &treeStruct_.getParent()->getPayload()->node_.transformContext;
    const auto& parentPos = parentTc->getPos();
    const auto& parentScale = parentTc->getScale();
    if (isHorizontalKnob_)
    {
        const float minX = parentPos.x;
        const float maxX = parentPos.x + parentScale.x - node_.transformContext.getScale().x;
        // https://rosettacode.org/wiki/Map_range
        // std::cout << sceneDataRef_.mouseOffsetFromFocusedCenter.x << "\n";
        setValue(((sceneDataRef_.mousePos.x + offsetFromCenter.x) - minX) / (maxX - minX));
    }
    else
    {
        const float minY = parentPos.y;
        const float maxY = parentPos.y + parentScale.y - node_.transformContext.getScale().y;
        // https://rosettacode.org/wiki/Map_range
        setValue(((sceneDataRef_.mousePos.y + offsetFromCenter.y) - minY) / (maxY - minY));
    }
}

void HkKnob::setValue(float value)
{
    value_ = value;
    if (value_ < 0.0f) value_ = 0.0f;
    if (value_ > 1.0f) value_ = 1.0f;
}

float HkKnob::getValue() const
{
    return value_;
}

bool HkKnob::isHorizontal() const
{
    return isHorizontalKnob_;
}
} // hkui