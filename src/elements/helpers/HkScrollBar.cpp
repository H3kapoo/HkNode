#include "HkScrollBar.hpp"

namespace hkui
{
HkScrollBar::HkScrollBar(const std::string& name, const bool isHorizontal)
    : HkNodeBase(name, HkNodeType::ScrollBar)
    , isHorizontal_(isHorizontal)
{
    node_.styleContext.setColor(glm::vec3(0.95f, 1.0f, 0.95f));

    knob_.styleContext.setColor(glm::vec3(0.5f, 0.7f, 0.9f));
    knob_.renderContext.colorUniformEn = true;

    /* default bar scale depending on orientation */
    setBarScale(barSize_);
}

void HkScrollBar::onFirstHeartbeat()
{
    std::string DEFAULT_VS = "assets/shaders/v1.glsl";
    std::string DEFAULT_FS = "assets/shaders/f1.glsl";
    const HkVertexArrayType DEFAULT_TYPE = HkVertexArrayType::QUAD;

    knob_.renderContext.shaderId = windowDataPtr_->renderer.addShaderSourceToCache(DEFAULT_VS, DEFAULT_FS);
    knob_.renderContext.vaoId = windowDataPtr_->renderer.addVertexArrayDataToCache(DEFAULT_TYPE);

    knob_.transformContext.setScale({ barSize_, barSize_ });
    knob_.transformContext.setPos(
        {
            node_.transformContext.getPos().x + node_.transformContext.getScale().x - barSize_,
            node_.transformContext.getPos().y + node_.transformContext.getScale().y - barSize_
        });

    HkNodeBase::onFirstHeartbeat();
}

void HkScrollBar::postRenderAdditionalDetails()
{
    knob_.renderContext.windowProjMatrix = windowDataPtr_->sceneProjMatrix;

    windowDataPtr_->renderer.render(knob_.renderContext,
        knob_.styleContext,
        knob_.transformContext.getModelMatrix());
}

void HkScrollBar::onScroll()
{
    setKnobValue(knobValue_ - windowDataPtr_->scrollPosY * scrollSensitivity_);
}

void HkScrollBar::onDrag()
{
    /* Parameters represent offsets from drag or click area so that mouse doesnt snap to center
       of object and instead keeps a natural distance from its center */
    computeKnobValue(windowDataPtr_->mouseOffsetFromFocusedCenter);
}

void HkScrollBar::onClick()
{
    computeKnobValue(
        {
            -knob_.transformContext.getScale().x * 0.5f,
            -knob_.transformContext.getScale().y * 0.5f
        });
}

void HkScrollBar::resolveChildrenConstraints(HkTreeStruct&, const HkScrollbarsSize&)
{
    node_.constraintContext.constrainSBKnob(
        isHorizontal_, overflowSize_, knobValue_, knob_.transformContext);
}

void HkScrollBar::setScrollValue(float value)
{
    (void)value;
}

/* Scale here refers to when a container has an overflow, how big of a margin this scrollbar will have. */
void HkScrollBar::setBarScale(uint32_t scale)
{
    if (isHorizontal_)
    {
        node_.transformContext.setScale({ node_.transformContext.getScale().x, scale });
    }
    /* Vertical*/
    else
    {
        node_.transformContext.setScale({ scale, node_.transformContext.getScale().y });
    }
}

/* Based on where the mouse is at drag/click/scroll time, compute 0 to 1 range mapped
   from scrollbar min pos to scrollbar max pos */
void HkScrollBar::computeKnobValue(const glm::ivec2 offsetFromCenter)
{
    if (isHorizontal_)
    {
        const float minX = node_.transformContext.getPos().x;
        const float maxX = node_.transformContext.getPos().x + node_.transformContext.getScale().x
            - knob_.transformContext.getScale().x;
        // https://rosettacode.org/wiki/Map_range
        setKnobValue(((windowDataPtr_->mousePos.x + offsetFromCenter.x) - minX) / (maxX - minX));
    }
    else
    {
        const float minY = node_.transformContext.getPos().y;
        const float maxY = node_.transformContext.getPos().y + node_.transformContext.getScale().y
            - knob_.transformContext.getScale().y;
        // https://rosettacode.org/wiki/Map_range
        setKnobValue(((windowDataPtr_->mousePos.y + offsetFromCenter.y) - minY) / (maxY - minY));
    }
}

void HkScrollBar::setKnobValue(float value)
{
    knobValue_ = value;
    if (knobValue_ < 0.0f) knobValue_ = 0.0f;
    if (knobValue_ > 1.0f) knobValue_ = 1.0f;
}

void HkScrollBar::setScrollBarActive(const bool isActive)
{
    isActive_ = isActive;
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
    return knobValue_;
}

bool HkScrollBar::isScrollBarActive() const
{
    return isActive_;
}
} // hkui