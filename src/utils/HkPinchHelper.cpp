#include "HkPinchHelper.hpp"

namespace hkui
{

void HkPinchHelper::init(HkWindowData& windowData)
{
    std::string DEFAULT_VS = "assets/shaders/v1.glsl";
    std::string DEFAULT_FS = "assets/shaders/f1.glsl";
    const HkVertexArrayType DEFAULT_TYPE = HkVertexArrayType::QUAD;

    pincher_.renderContext.shaderId = windowData.renderer.addShaderSourceToCache(DEFAULT_VS, DEFAULT_FS);
    pincher_.renderContext.vaoId = windowData.renderer.addVertexArrayDataToCache(DEFAULT_TYPE);

    pincher_.styleContext.setColor(glm::vec3(0.2f, 0.2f, 0.2f));
    pincher_.renderContext.colorUniformEn = true;
}

void HkPinchHelper::onMove(HkWindowData& windowData, glm::ivec2& boundPos, glm::ivec2& boundScale)
{
    /* We are just moving around, scanning for a potential grab point */
    if (!windowData.isMouseClicked)
    {
        const auto mousePos = windowData.mousePos;
        const auto nodeEndPos = boundPos + boundScale;

        const bool TZone = (mousePos.y > boundPos.y - grabSize_)
            && (mousePos.y < boundPos.y);
        const bool BZone = (mousePos.y < nodeEndPos.y + grabSize_)
            && (mousePos.y > nodeEndPos.y);
        const bool LZone = (mousePos.x > boundPos.x - grabSize_)
            && (mousePos.x < boundPos.x);
        const bool RZone = (mousePos.x > nodeEndPos.x)
            && (mousePos.x < nodeEndPos.x + grabSize_);
        const bool VBound = mousePos.y > boundPos.y &&
            mousePos.y < nodeEndPos.y;
        const bool HBound = mousePos.x > boundPos.x &&
            mousePos.x < nodeEndPos.x;

        // pinch right
        if (RZone && VBound)
        {
            lockedInXR_ = true;
        }
        else
        {
            lockedInXR_ = false;
        }

        // pinch left
        if (LZone && VBound)
        {
            lockedInXL_ = true;
        }
        else
        {
            lockedInXL_ = false;
        }

        // pinch top
        if (TZone && HBound)
        {
            lockedInYT_ = true;
        }
        else
        {
            lockedInYT_ = false;
        }

        // pinch bottom
        if (BZone && HBound)
        {
            lockedInYB_ = true;
        }
        else
        {
            lockedInYB_ = false;
        }

        // diagonal bottom-right pinch
        if (RZone && BZone)
        {
            lockedInXR_ = true;
            lockedInYB_ = true;
        }

        // diagonal top-right pinch
        if (RZone && TZone)
        {
            lockedInXR_ = true;
            lockedInYT_ = true;
        }

        // diagonal bottom-left pinch
        if (LZone && BZone)
        {
            lockedInXL_ = true;
            lockedInYB_ = true;
        }

        // diagonal top-left pinch
        if (LZone && TZone)
        {
            lockedInXL_ = true;
            lockedInYT_ = true;
        }
    }

    if (lockedInXR_ && lockedInYB_)
    {
        cursorChange(windowData, GLFW_CROSSHAIR_CURSOR);
        if (windowData.isMouseClicked)
        {
            boundScale.x += windowData.mousePos.x - windowData.lastMousePos.x;
            boundScale.y += windowData.mousePos.y - windowData.lastMousePos.y;
        }
        return;
    }

    if (lockedInXR_ && lockedInYT_)
    {
        cursorChange(windowData, GLFW_CROSSHAIR_CURSOR);
        if (windowData.isMouseClicked)
        {
            boundScale.x += windowData.mousePos.x - windowData.lastMousePos.x;
            boundPos.y += windowData.mousePos.y - windowData.lastMousePos.y;
            boundScale.y += -(windowData.mousePos.y - windowData.lastMousePos.y);
        }
        return;
    }

    if (lockedInXL_ && lockedInYB_)
    {
        cursorChange(windowData, GLFW_CROSSHAIR_CURSOR);
        if (windowData.isMouseClicked)
        {
            boundPos.x += windowData.mousePos.x - windowData.lastMousePos.x;
            boundScale.x += -(windowData.mousePos.x - windowData.lastMousePos.x);
            boundScale.y += (windowData.mousePos.y - windowData.lastMousePos.y);
        }
        return;
    }

    if (lockedInXL_ && lockedInYT_)
    {
        cursorChange(windowData, GLFW_CROSSHAIR_CURSOR);
        if (windowData.isMouseClicked)
        {
            boundPos.x += windowData.mousePos.x - windowData.lastMousePos.x;
            boundScale.x += -(windowData.mousePos.x - windowData.lastMousePos.x);
            boundPos.y += windowData.mousePos.y - windowData.lastMousePos.y;
            boundScale.y += -(windowData.mousePos.y - windowData.lastMousePos.y);
        }
        return;
    }

    if (lockedInXR_)
    {
        cursorChange(windowData, GLFW_HRESIZE_CURSOR);
        if (windowData.isMouseClicked)
        {
            boundScale.x += windowData.mousePos.x - windowData.lastMousePos.x;
        }
    }

    if (lockedInXL_)
    {
        cursorChange(windowData, GLFW_HRESIZE_CURSOR);
        if (windowData.isMouseClicked)
        {
            boundPos.x += windowData.mousePos.x - windowData.lastMousePos.x;
            boundScale.x += -(windowData.mousePos.x - windowData.lastMousePos.x);
        }
    }

    if (lockedInYT_)
    {
        cursorChange(windowData, GLFW_VRESIZE_CURSOR);
        if (windowData.isMouseClicked)
        {
            boundPos.y += windowData.mousePos.y - windowData.lastMousePos.y;
            boundScale.y += -(windowData.mousePos.y - windowData.lastMousePos.y);
        }
    }

    if (lockedInYB_)
    {
        cursorChange(windowData, GLFW_VRESIZE_CURSOR);
        if (windowData.isMouseClicked)
        {
            boundScale.y += (windowData.mousePos.y - windowData.lastMousePos.y);
        }
    }

    /* If we didn't manage to grab anything, reset cursor */
    if (!isSomethingActive())
    {
        cursorChange(windowData, GLFW_ARROW_CURSOR);
    }
}

void HkPinchHelper::onBarRender(HkWindowData& windowData, const glm::ivec2 boundPos, const glm::ivec2 boundScale)
{
    const int32_t extendT = lockedInYT_ ? grabSize_ : 0;
    const int32_t extendB = lockedInYB_ ? grabSize_ : 0;

    pincher_.renderContext.windowProjMatrix = windowData.sceneProjMatrix;

    if (lockedInXR_)
    {
        glScissor(
            boundPos.x,
            windowData.windowSize.y - boundPos.y - boundScale.y - extendB,
            boundScale.x + grabSize_,
            boundScale.y + 2 * extendT + extendB);
        pincher_.transformContext.setScale(
            {
                grabSize_,
                boundScale.y + extendT + extendB
            });
        pincher_.transformContext.setPos(
            {
                boundPos.x + boundScale.x,
                boundPos.y - extendT
            });

        windowData.renderer.render(pincher_.renderContext,
            pincher_.styleContext,
            pincher_.transformContext.getModelMatrix());
    }

    if (lockedInXL_)
    {
        glScissor(
            boundPos.x - grabSize_,
            windowData.windowSize.y - boundPos.y - boundScale.y - extendB,
            boundScale.x + grabSize_,
            boundScale.y + 2 * extendT + extendB);
        pincher_.transformContext.setScale(
            {
                grabSize_,
                boundScale.y + extendT + extendB
            });
        pincher_.transformContext.setPos(
            {
                boundPos.x - grabSize_,
                boundPos.y - extendT
            });

        windowData.renderer.render(pincher_.renderContext,
            pincher_.styleContext,
            pincher_.transformContext.getModelMatrix());
    }

    if (lockedInYT_)
    {
        glScissor(
            boundPos.x,
            windowData.windowSize.y - boundPos.y - boundScale.y,
            boundScale.x,
            boundScale.y + grabSize_);
        pincher_.transformContext.setScale(
            {
                boundScale.x,
                grabSize_
            });
        pincher_.transformContext.setPos(
            {
                boundPos.x,
                boundPos.y - grabSize_
            });

        windowData.renderer.render(pincher_.renderContext,
            pincher_.styleContext,
            pincher_.transformContext.getModelMatrix());
    }

    if (lockedInYB_)
    {
        glScissor(
            boundPos.x,
            windowData.windowSize.y - boundPos.y - boundScale.y - grabSize_,
            boundScale.x,
            boundScale.y);
        pincher_.transformContext.setScale(
            {
                boundScale.x,
                grabSize_
            });
        pincher_.transformContext.setPos(
            {
                boundPos.x,
                boundPos.y + boundScale.y
            });

        windowData.renderer.render(pincher_.renderContext,
            pincher_.styleContext,
            pincher_.transformContext.getModelMatrix());
    }
}

void HkPinchHelper::cursorChange(HkWindowData& windowData, const int32_t value)
{
    windowData.suggestedCursor = value;
    windowData.cursorChangeNeeded = true;
}

bool HkPinchHelper::isSomethingActive()
{
    if (lockedInXR_ == false && lockedInXL_ == false && lockedInYT_ == false && lockedInYB_ == false)
        return false;
    return true;
}

void HkPinchHelper::setGrabSize(const int32_t size) { grabSize_ = size; }
} // hkui