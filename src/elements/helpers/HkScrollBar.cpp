#include "HkScrollBar.hpp"
#include "../../APIGate/GlfwGlewGate.hpp"

namespace hkui
{
HkScrollBar::HkScrollBar(const std::string& name, const bool isHorizontal)
    : HkNodeBase(name, HkNodeType::ScrollBar)
    , isHorizontal_(isHorizontal)
{
    node_.styleContext.setColor(glm::vec4(0.95f, 1.0f, 0.95f, 1.0f));

    knob_.styleContext.setColor(glm::vec4(0.5f, 0.7f, 0.9f, 1.0f));
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

    knob_.transformContext.setContentScale({ barSize_, barSize_ });
    knob_.transformContext.setContentPos(
        {
            node_.transformContext.getContentPos().x + node_.transformContext.getContentScale().x - barSize_,
            node_.transformContext.getContentPos().y + node_.transformContext.getContentScale().y - barSize_
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
    isAnimOngoing = true;
    restarted = true;
    startPos = knobValue_;
    endPos = knobValue_ - windowDataPtr_->scrollPosY * scrollSensitivity_;
    setKnobValue(knobValue_ - windowDataPtr_->scrollPosY * scrollSensitivity_);
}

void HkScrollBar::onDrag()
{
    /* Parameters represent offsets from drag or click area so that mouse doesnt snap to center
       of object and instead keeps a natural distance from its center */
    isAnimOngoing = true;
    restarted = true;
    startPos = knobValue_;
    computeKnobValue((-(knob_.transformContext.getScale()) / 2));
    endPos = knobValue_;
}

void HkScrollBar::onClick()
{
    /* Only reposition knob if we click outside of it*/
    //TODO: isPosInsideOfNode is based on asbolute pos, maybe contentPos is needed instead?
    if (!knob_.transformContext.isPosInsideOfNode(windowDataPtr_->mousePos))
    {
        computeKnobValue((-(knob_.transformContext.getContentScale()) / 2));
    }
}

void HkScrollBar::onAnimationFrameRequested()
{
    if (!isAnimOngoing) return;

    if (restarted)
    {
        t = 0.0f;
        restarted = false;
        startTime = glfwGetTime();
    }

    double currentTime = glfwGetTime();
    double elapsedTime = currentTime - startTime;
    t = elapsedTime / animDuration;

    if (t > 1.0f)
    {
        t = 1.0f;
        isAnimOngoing = false;
    }

    const auto easeInOutCubic = [](double x) -> double {
        const double c1 = 1.70158;
        const double c3 = c1 + 1.0;
        return 1.0 + c3 * std::pow(x - 1.0, 3.0) + c1 * std::pow(x - 1.0, 2.0);};

    float interpPos = startPos + (endPos - startPos) * (float)easeInOutCubic(t);
    setKnobValue(interpPos);
    glfwPostEmptyEvent(); //TODO: This should be posted ONLY once per frame
}

void HkScrollBar::resolveChildrenConstraints(HkTreeStruct&, const HkScrollbarsSize&)
{
    node_.constraintContext.constrainSBKnob(
        isHorizontal_, overflowSize_, knobValue_, knob_.transformContext);
}

void HkScrollBar::setScrollValue(float value)
{
    setKnobValue(value);
}

/* Scale here refers to when a container has an overflow, how big of a margin this scrollbar will have. */
void HkScrollBar::setBarScale(uint32_t scale)
{
    if (isHorizontal_)
    {
        node_.transformContext.setContentScale({ node_.transformContext.getContentScale().x, scale });
    }
    /* Vertical*/
    else
    {
        node_.transformContext.setContentScale({ scale, node_.transformContext.getContentScale().y });
    }
}

/* Based on where the mouse is at drag/click/scroll time, compute 0 to 1 range mapped
   from scrollbar min pos to scrollbar max pos taking into accound the knob size*/
void HkScrollBar::computeKnobValue(const glm::ivec2 offsetFromCenter)
{
    if (isHorizontal_)
    {
        const float minX = node_.transformContext.getContentPos().x;
        const float maxX = node_.transformContext.getContentPos().x + node_.transformContext.getContentScale().x
            - knob_.transformContext.getContentScale().x;
        // https://rosettacode.org/wiki/Map_range
        setKnobValue(((windowDataPtr_->mousePos.x + offsetFromCenter.x) - minX) / (maxX - minX));
    }
    else
    {
        const float minY = node_.transformContext.getContentPos().y;
        const float maxY = node_.transformContext.getContentPos().y + node_.transformContext.getContentScale().y
            - knob_.transformContext.getContentScale().y;
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