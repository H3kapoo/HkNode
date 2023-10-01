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
}

void HkKnob::onFirstHeartbeat()
{
    const std::string DEFAULT_VS = "assets/shaders/v1.glsl";
    const std::string DEFAULT_FS = "assets/shaders/f1.glsl";
    const HkVertexArrayType DEFAULT_TYPE = HkVertexArrayType::QUAD;
    node_.renderContext.shaderId = windowDataPtr_->renderer.addShaderSourceToCache(DEFAULT_VS, DEFAULT_FS);
    node_.renderContext.vaoId = windowDataPtr_->renderer.addVertexArrayDataToCache(DEFAULT_TYPE);
}

void HkKnob::onDrag()
{
    computeKnobValue(windowDataPtr_->mouseOffsetFromFocusedCenter);
}

void HkKnob::onScroll()
{
    scrollOngoing();
}

void HkKnob::scrollOngoing()
{
    setValue(getValue() - windowDataPtr_->scrollPosY * scrollSensitivity_);
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
        // std::cout << windowDataPtr_->mouseOffsetFromFocusedCenter.x << "\n";
        setValue(((windowDataPtr_->mousePos.x + offsetFromCenter.x) - minX) / (maxX - minX));
    }
    else
    {
        const float minY = parentPos.y;
        const float maxY = parentPos.y + parentScale.y - node_.transformContext.getScale().y;
        // https://rosettacode.org/wiki/Map_range
        setValue(((windowDataPtr_->mousePos.y + offsetFromCenter.y) - minY) / (maxY - minY));
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