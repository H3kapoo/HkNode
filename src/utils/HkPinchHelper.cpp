#include "HkPinchHelper.hpp"

// #include <ranges>
namespace hkui
{


std::vector<HkPinchHelper::PinchInfo> HkPinchHelper::pinchInfo_{ 0 };
std::vector<HkPinchHelper::PinchInfo> HkPinchHelper::validityGroup_{ 0 };
bool HkPinchHelper::resolved_{ false };

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

void HkPinchHelper::clear()
{
    pinchInfo_.clear();
    validityGroup_.clear();
    resolved_ = false;
}

void HkPinchHelper::resolve()
{
    if (resolved_) return;
    resolved_ = true;

    std::sort(pinchInfo_.begin(), pinchInfo_.end(), [](const PinchInfo& a, const PinchInfo& b)
        {
            return a.level < b.level;
        });

    // just for debug print
    for (const auto& pi : pinchInfo_)
    {
        break;
        std::cout << pi.nodeId << " "
            << pi.level << " L:"
            << pi.left << "  R:"
            << pi.right << "  T:"
            << "  " << pi.top << "  B:"
            << pi.bottom << "\n";
    }

    /*
        T+B => R L no matter how many
        R+L => T B no matter how many
    */
    uint32_t currentLevel{ 0 };
    bool topNeeded{ true };
    bool botNeeded{ true };
    bool leftNeeded{ true };
    bool rightNeeded{ true };
    bool topBottomAquired{ false };
    bool leftRightAquired{ false };
    for (const auto& pi : pinchInfo_)
    {
        if (currentLevel == 0 && (pi.left || pi.right || pi.top || pi.bottom))
        {
            currentLevel = pi.level;
        }

        if (currentLevel != 0 && currentLevel != pi.level)
        {
            std::cout << "Nothing more on level of interest: " << currentLevel << "\n";
            break;
        }

        PinchInfo validInfo;
        bool okToPush{ false };
        if (pi.bottom && botNeeded)
        {
            validInfo.bottom = true;
            okToPush = true;
            botNeeded = false;
            topBottomAquired = true;
        }

        if (pi.top && topNeeded)
        {
            validInfo.top = true;
            okToPush = true;
            topNeeded = false;
            topBottomAquired = true;
        }

        if (pi.left && leftNeeded)
        {
            validInfo.left = true;
            okToPush = true;
            leftNeeded = false;
            leftRightAquired = true;
        }

        if (pi.right && rightNeeded)
        {
            validInfo.right = true;
            okToPush = true;
            rightNeeded = false;
            leftRightAquired = true;
        }

        if (okToPush)
        {
            validInfo.level = pi.level;
            validInfo.nodeId = pi.nodeId;
            validityGroup_.push_back(validInfo);
        }
    }

    //Toate L and R de la urmatoru nivel doar
    // and reverse now
    // std::reverse(pinchInfo_.begin(), pinchInfo_.end());
    uint32_t prevLevel = currentLevel;
    currentLevel = 0;
    for (const auto& pi : pinchInfo_)
    {
        if (currentLevel == 0 && pi.level > prevLevel && (pi.left || pi.right || pi.top || pi.bottom))
        {
            currentLevel = pi.level;
        }

        if (currentLevel != 0 && currentLevel != pi.level)
        {
            std::cout << "Nothing more on level of interest: " << currentLevel << "\n";
            break;
        }


        PinchInfo validInfo;
        bool okToPush{ false };
        if (pi.bottom && !topBottomAquired)
        {
            validInfo.bottom = true;
            okToPush = true;
            // botNeeded = false;
        }

        if (pi.top && !topBottomAquired)
        {
            validInfo.top = true;
            okToPush = true;
            // topNeeded = false;
        }

        if (pi.left && !leftRightAquired)
        {
            validInfo.left = true;
            okToPush = true;
            // leftNeeded = false;
        }

        if (pi.right && !leftRightAquired)
        {
            validInfo.right = true;
            okToPush = true;
            // rightNeeded = false;
        }

        if (okToPush)
        {
            validInfo.level = pi.level;
            validInfo.nodeId = pi.nodeId;
            validityGroup_.push_back(validInfo);
        }
    }

    // // we need to sort it and find the entry that repeats only once and we should remove it
    // std::sort(validityGroup_.begin(), validityGroup_.end(), [](const PinchInfo& a, const PinchInfo& b)
    //     {
    //         return a.level < b.level;
    //     });

    // uint32_t rmIndex = 0;
    // for (uint32_t i = 0; i < validityGroup_.size();)
    // {
    //     // last element
    //     if (i == validityGroup_.size() - 1)
    //         rmIndex = i;

    //     uint32_t j = i;
    //     while (validityGroup_[i].level == validityGroup_[j].level && j < validityGroup_.size())
    //     {
    //         ++j;
    //     }

    //     if (j - i == 1)
    //         rmIndex = i;
    //     else
    //         i = j;
    // }

    // std::cout << "Rm index is: " << rmIndex << "\n";

    for (const auto& pi : validityGroup_)
    {
        std::cout << "valid info: ";
        std::cout << pi.nodeId << " "
            << pi.level << " L:"
            << pi.left << "  R:"
            << pi.right << "  T:"
            << "  " << pi.top << "  B:"
            << pi.bottom << "\n";
    }
}

void HkPinchHelper::scan(HkWindowData& windowData, glm::ivec2& boundPos, glm::ivec2& boundScale,
    const uint32_t id, const uint32_t level)
{
    PinchInfo pi;
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
    if (RZone && VBound) pi.right = true;
    // pinch left
    if (LZone && VBound) pi.left = true;
    // pinch top
    if (TZone && HBound) pi.top = true;
    // pinch bottom
    if (BZone && HBound) pi.bottom = true;

    // diagonal bottom-right pinch
    if (RZone && BZone) { pi.right = true;pi.bottom = true; }
    // diagonal top-right pinch
    if (RZone && TZone) { pi.right = true; pi.top = true; }
    // diagonal bottom-left pinch
    if (LZone && BZone) { pi.left = true; pi.bottom = true; }
    // diagonal top-left pinch
    if (LZone && TZone) { pi.left = true;pi.top = true; }

    pi.nodeId = id;
    pi.level = level;
    pinchInfo_.push_back(pi);
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

        //TODO: in non subWindow mode, both sides need to be active. BUt only in this mode
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

    // if (lockedInXR_ && lockedInYB_)
    // {
    //     cursorChange(windowData, GLFW_CROSSHAIR_CURSOR);
    //     if (windowData.isMouseClicked)
    //     {
    //         boundScale.x += windowData.mousePos.x - windowData.lastMousePos.x;
    //         boundScale.y += windowData.mousePos.y - windowData.lastMousePos.y;
    //     }
    //     return;
    // }

    // if (lockedInXR_ && lockedInYT_)
    // {
    //     cursorChange(windowData, GLFW_CROSSHAIR_CURSOR);
    //     if (windowData.isMouseClicked)
    //     {
    //         boundScale.x += windowData.mousePos.x - windowData.lastMousePos.x;
    //         boundPos.y += windowData.mousePos.y - windowData.lastMousePos.y;
    //         boundScale.y += -(windowData.mousePos.y - windowData.lastMousePos.y);
    //     }
    //     return;
    // }

    // if (lockedInXL_ && lockedInYB_)
    // {
    //     cursorChange(windowData, GLFW_CROSSHAIR_CURSOR);
    //     if (windowData.isMouseClicked)
    //     {
    //         boundPos.x += windowData.mousePos.x - windowData.lastMousePos.x;
    //         boundScale.x += -(windowData.mousePos.x - windowData.lastMousePos.x);
    //         boundScale.y += (windowData.mousePos.y - windowData.lastMousePos.y);
    //     }
    //     return;
    // }

    // if (lockedInXL_ && lockedInYT_)
    // {
    //     cursorChange(windowData, GLFW_CROSSHAIR_CURSOR);
    //     if (windowData.isMouseClicked)
    //     {
    //         boundPos.x += windowData.mousePos.x - windowData.lastMousePos.x;
    //         boundScale.x += -(windowData.mousePos.x - windowData.lastMousePos.x);
    //         boundPos.y += windowData.mousePos.y - windowData.lastMousePos.y;
    //         boundScale.y += -(windowData.mousePos.y - windowData.lastMousePos.y);
    //     }
    //     return;
    // }

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
        // std::cout << glfwGetTime() << " fffff\n";
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