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

void HkPinchHelper::scan(HkWindowData& windowData, HkNodeData& nd,
    const uint32_t id, const uint32_t level)
{
    /* Each click has first a focusScan pass. We can use this to scan all objects that support pinching
      for potential pinch points and add them to a potential final pinch vector*/
    PinchInfo pi;
    auto& tc = nd.transformContext;
    const auto mousePos = windowData.mousePos;
    const auto nodeEndPos = tc.getVPos() + tc.getVScale();

    const bool TZone = (mousePos.y > tc.getVPos().y - grabSize_)
        && (mousePos.y < tc.getVPos().y);
    const bool BZone = (mousePos.y < nodeEndPos.y + grabSize_)
        && (mousePos.y > nodeEndPos.y);
    const bool LZone = (mousePos.x > tc.getVPos().x - grabSize_)
        && (mousePos.x < tc.getVPos().x);
    const bool RZone = (mousePos.x > nodeEndPos.x)
        && (mousePos.x < nodeEndPos.x + grabSize_);
    const bool VBound = mousePos.y > tc.getVPos().y &&
        mousePos.y < nodeEndPos.y;
    const bool HBound = mousePos.x > tc.getVPos().x &&
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

void HkPinchHelper::onMouseButton(HkWindowData& windowData)
{
    /* On click (after focusPass), resolve which pinchInfos need to be used.
       On release, clear cache */
    windowData.isMouseClicked ? resolve() : clear();
}

void HkPinchHelper::resolve()
{
    /* If we already resolved the pinchInfos for this click, bail out */
    if (resolved_) return;
    resolved_ = true;

    /* Sort the scanned pinch zones by ascending level */
    std::sort(pinchInfo_.begin(), pinchInfo_.end(),
        [](const PinchInfo& a, const PinchInfo& b) {return a.level < b.level;});

    /* We need two passes to aquire the needed pinch info for pinch to work.
      First pass aquires the first B+T or R+L combination scanning from the lower levels upwards.
      The second pass aquires all the opposite pairs (or singles) from the found one, starting at
      the opposite pair's level +1*/
    bool topNeeded{ true };
    bool botNeeded{ true };
    bool leftNeeded{ true };
    bool rightNeeded{ true };
    bool topBottomAquired{ false };
    bool leftRightAquired{ false };
    uint32_t currentLevel{ 0 };

    /* First Pass*/
    for (const auto& pi : pinchInfo_)
    {
        if (currentLevel == 0 && (pi.left || pi.right || pi.top || pi.bottom))
            currentLevel = pi.level;
        if (currentLevel != 0 && currentLevel != pi.level)
            break;

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

    /* Second Pass. Restart level and start at previous level+1 */
    uint32_t prevLevel = currentLevel;
    currentLevel = 0;
    for (const auto& pi : pinchInfo_)
    {
        if (currentLevel == 0 && pi.level > prevLevel
            && (pi.left || pi.right || pi.top || pi.bottom))
            currentLevel = pi.level;
        if (currentLevel != 0 && currentLevel != pi.level)
            break;

        PinchInfo validInfo;
        bool okToPush{ false };
        if (pi.bottom && !topBottomAquired) { validInfo.bottom = true;okToPush = true; }
        if (pi.top && !topBottomAquired) { validInfo.top = true;okToPush = true; }
        if (pi.left && !leftRightAquired) { validInfo.left = true;okToPush = true; }
        if (pi.right && !leftRightAquired) { validInfo.right = true;okToPush = true; }

        if (okToPush)
        {
            validInfo.level = pi.level;
            validInfo.nodeId = pi.nodeId;
            validityGroup_.push_back(validInfo);
        }
    }

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


void HkPinchHelper::onMouseMove(HkWindowData& windowData, HkNodeData& nd, HkNodeData& pnd, const uint32_t id)
{
    /* We are just moving around, scan for grab points in order to display grabBars to user. These will
       not be used for position calculations*/
    auto& tc = nd.transformContext;
    if (!windowData.isMouseClicked)
    {
        const auto mousePos = windowData.mousePos;
        const auto nodeEndPos = tc.getVPos() + tc.getVScale();

        const bool TZone = (mousePos.y > tc.getVPos().y - grabSize_)
            && (mousePos.y < tc.getVPos().y);
        const bool BZone = (mousePos.y < nodeEndPos.y + grabSize_)
            && (mousePos.y > nodeEndPos.y);
        const bool LZone = (mousePos.x > tc.getVPos().x - grabSize_)
            && (mousePos.x < tc.getVPos().x);
        const bool RZone = (mousePos.x > nodeEndPos.x)
            && (mousePos.x < nodeEndPos.x + grabSize_);
        const bool VBound = mousePos.y > tc.getVPos().y &&
            mousePos.y < nodeEndPos.y;
        const bool HBound = mousePos.x > tc.getVPos().x &&
            mousePos.x < nodeEndPos.x;

        // pinch right
        if (RZone && VBound) lockedInXR_ = true; else lockedInXR_ = false;
        // pinch left
        if (LZone && VBound) lockedInXL_ = true; else lockedInXL_ = false;
        // pinch top
        if (TZone && HBound) lockedInYT_ = true; else lockedInYT_ = false;
        // pinch bottom
        if (BZone && HBound) lockedInYB_ = true; else lockedInYB_ = false;
        // diagonal bottom-right pinch
        if (RZone && BZone) { lockedInXR_ = true; lockedInYB_ = true; }
        // diagonal top-right pinch
        if (RZone && TZone) { lockedInXR_ = true; lockedInYT_ = true; }
        // diagonal bottom-left pinch
        if (LZone && BZone) { lockedInXL_ = true; lockedInYB_ = true; }
        // diagonal top-left pinch
        if (LZone && TZone) { lockedInXL_ = true; lockedInYT_ = true; }
    }
    /* If we are holding click and moving, we need to update position/scale of pinched object */
    else
    {
        bool isValid = false;
        PinchInfo foundInfo;
        for (const auto& pi : validityGroup_)
        {
            if (id == pi.nodeId)
            {
                isValid = true;
                foundInfo = pi;
                break;
            }
        }

        if (!isValid) return;

        //TODO: make them local
        glm::vec2 boundPos = { tc.getPos() };
        glm::vec2 boundScale = { tc.getScale() };

        if (foundInfo.right)
        {
            boundScale.x += windowData.mousePos.x - windowData.lastMousePos.x;
        }

        if (foundInfo.left)
        {
            boundPos.x += windowData.mousePos.x - windowData.lastMousePos.x;
            boundScale.x += -(windowData.mousePos.x - windowData.lastMousePos.x);
        }

        if (foundInfo.top)
        {
            boundPos.y += windowData.mousePos.y - windowData.lastMousePos.y;
            boundScale.y += -(windowData.mousePos.y - windowData.lastMousePos.y);
        }

        if (foundInfo.bottom)
        {
            boundScale.y += (windowData.mousePos.y - windowData.lastMousePos.y);
        }

        const auto pScaleX = pnd.transformContext.getScale().x;
        const auto pScaleY = pnd.transformContext.getScale().y;
        const float percX = ((float)boundScale.x - tc.getScale().x) / pScaleX;
        const float percY = ((float)boundScale.y - tc.getScale().y) / pScaleY;

        tc.setPos(boundPos);
        auto prev = nd.styleContext.getHSizeConfig();
        prev.value += percX;

        auto prevY = nd.styleContext.getVSizeConfig();
        prevY.value += percY;
        nd.styleContext.setHSizeConfig(prev);
        nd.styleContext.setVSizeConfig(prevY);
    }
}

void HkPinchHelper::onBarRender(HkWindowData& windowData, const glm::ivec2 boundPos, const glm::ivec2 boundScale)
{
    const int32_t extendT = lockedInYT_ ? grabSize_ : 0;
    const int32_t extendB = lockedInYB_ ? grabSize_ : 0;

    if (boundScale.x == 0 || boundScale.y == 0)
        return;

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

void HkPinchHelper::clear()
{
    /* If we already resolved something, clear on mouse release */
    if (!resolved_) return;

    /* Restore everything after click release */
    pinchInfo_.clear();
    validityGroup_.clear();
    resolved_ = false;
}

bool HkPinchHelper::onMouseMoveCustom(HkWindowData& windowData, glm::ivec2& boundPos, glm::ivec2& boundScale)
{
    /* This is made custom for windowFrame as logic there is a bit different and not so complex like
       for containers*/

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
        if (RZone && VBound) lockedInXR_ = true; else lockedInXR_ = false;
        // pinch left
        if (LZone && VBound) lockedInXL_ = true; else lockedInXL_ = false;
        // pinch top
        if (TZone && HBound) lockedInYT_ = true; else lockedInYT_ = false;
        // pinch bottom
        if (BZone && HBound) lockedInYB_ = true; else lockedInYB_ = false;
        // diagonal bottom-right pinch
        if (RZone && BZone) { lockedInXR_ = true; lockedInYB_ = true; }
        // diagonal top-right pinch
        if (RZone && TZone) { lockedInXR_ = true; lockedInYT_ = true; }
        // diagonal bottom-left pinch
        if (LZone && BZone) { lockedInXL_ = true; lockedInYB_ = true; }
        // diagonal top-left pinch
        if (LZone && TZone) { lockedInXL_ = true; lockedInYT_ = true; }

        /* Set cursor to whatever we might need */
        if (lockedInXR_ || lockedInXL_)
            cursorChange(windowData, GLFW_HRESIZE_CURSOR);

        if (lockedInYT_ || lockedInYB_)
            cursorChange(windowData, GLFW_VRESIZE_CURSOR);

        if ((lockedInXR_ && lockedInYB_) || (lockedInXR_ && lockedInYT_)
            || (lockedInXL_ && lockedInYB_) || (lockedInXL_ && lockedInYT_))
            cursorChange(windowData, GLFW_CROSSHAIR_CURSOR);

        /* If we didn't manage to grab anything, reset cursor */
        if (!(lockedInXL_ || lockedInXR_ || lockedInYB_ || lockedInYT_))
            cursorChange(windowData, GLFW_ARROW_CURSOR);

        /* Return false as this is "scan" pass (maybe separate func in the future), not "resolve" one */
        return false;
    }
    /* (resolve pass) Is mouse is clicked and we are dragging, see if there's something changed so
       we can update */
    else
    {
        if (lockedInXR_)
        {
            boundScale.x += windowData.mousePos.x - windowData.lastMousePos.x;
        }

        if (lockedInXL_)
        {
            boundPos.x += windowData.mousePos.x - windowData.lastMousePos.x;
            boundScale.x += -(windowData.mousePos.x - windowData.lastMousePos.x);
        }

        if (lockedInYT_)
        {
            boundPos.y += windowData.mousePos.y - windowData.lastMousePos.y;
            boundScale.y += -(windowData.mousePos.y - windowData.lastMousePos.y);
        }

        if (lockedInYB_)
        {
            boundScale.y += (windowData.mousePos.y - windowData.lastMousePos.y);
        }

        /* Tell windowFrame if something needs update */
        return lockedInXL_ || lockedInXR_ || lockedInYB_ || lockedInYT_;
    }
}

void HkPinchHelper::cursorChange(HkWindowData& windowData, const int32_t value)
{
    windowData.suggestedCursor = value;
    windowData.cursorChangeNeeded = true;
}

void HkPinchHelper::setGrabSize(const int32_t size) { grabSize_ = size; }
} // hkui