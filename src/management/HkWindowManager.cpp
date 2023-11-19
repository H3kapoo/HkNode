#include "HkWindowManager.hpp"

#include <iostream>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

namespace hkui
{

uint32_t HkWindowManager::idGiver = 1;

HkWindowManager::HkWindowManager(const std::string& windowName, const HkWindowConfig config)
{
    //TODO: Ability to pass options to ctor
    windowHandle_ = glfwCreateWindow(config.width, config.height, windowName.c_str(), NULL, NULL);

    if (windowHandle_ == NULL)
    {
        std::cerr << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return;
    }

    /* Make any newly created window the currently bound context to make sure
       future glew/glfw API calls succeed */
    glfwMakeContextCurrent(windowHandle_);
    glfwSwapInterval(0); // zero to disable Vsync

    windowData_.lastMousePos = windowData_.mousePos = { 0,0 };
    windowData_.focusedId = HkWindowData::NO_SELECTION_ID;
    windowData_.hoveredId = HkWindowData::NO_SELECTION_ID;

    windowData_.windowSize = { config.width, config.height };
    windowData_.sceneProjMatrix = glm::ortho(0.0f, (float)config.width, (float)config.height, 0.0f, 0.0f, 100.0f);
    windowData_.windowHandle = windowHandle_;

    windowName_ = windowName;
    id_ = idGiver++;
    isMaster_ = config.isMaster;

    windowData_.windowId = id_;
}

//TODO: Rename appropriately in the future
void HkWindowManager::forceUpdate()
{
    // updateAllSubWindows(HkEvent::None);

    /* We could treat this force update as a request for an animation frame*/
    updateAllSubWindows(HkEvent::AnimationFrame);
}

void HkWindowManager::updateAllSubWindows(const HkEvent& ev)
{
    /* Firstly load the background image if needed */
    renderBackgroundImage();

    /* Focus scan event is special here. We need to find the first subWindow that the mouse click and do so from front
    most subWindow to back most one (reverse iteration, last element in vector is always the frontmost window). Once we
    found a fist match, we bail out. In case we don't find any suitable subWindow, we return from function,
    there's nothing left to be done.*/
    windowData_.currentEvent = ev;
    if (windowData_.previousTime == 0.0f)
    {
        windowData_.previousTime = glfwGetTime();
        std::cout << "enterd here\n";
    }

    if (windowData_.currentEvent == HkEvent::FocusScan)
    {
        std::cout << "Number of subwindows: " << rootSubWindows_.size() << "\n";
        for (int32_t i = rootSubWindows_.size() - 1; i >= 0; i--)
        {
            //TODO: Could we pass a RENDERER object here who has all the bind for the current window?
            rootSubWindows_[i]->rootUpdate(); // TODO: Here it's not mandatory to render. Small optimization
            if (windowData_.focusedId != HkWindowData::NO_SELECTION_ID)
            {
                windowData_.focusedSubWindowId = i;

                /* Logic example:
                    1 3 2 4   - window order, we want to make '3' the focused one
                    1 3 2 4 3 - add focus to the end again
                    1 2 4 3   -delete pos where focused was before

                    This nicely preserves order of untoched windows.
                */
                rootSubWindows_.push_back(rootSubWindows_[windowData_.focusedSubWindowId]);
                rootSubWindows_.erase(rootSubWindows_.begin() + windowData_.focusedSubWindowId);
                return;
            }
        }
        return;
    }

    for (auto it = rootSubWindows_.begin(); it != rootSubWindows_.end();)
    {
        if ((*it)->isAlive())
        {
            /* Update subwindow */
            (*it)->rootUpdate();

            decideCursor();
            ++it;
        }
        else
        {
            std::cout << "SubWindow '" << (*it)->getUnderlayingNode()->getNodeInfo().name << "' requested close.\n";
            /* This will not deallocate the pointer */
            it = rootSubWindows_.erase(it);
        }
    }

    //EXPERIMENTAL, MAYBE NEEDS REMOVAL
    // double currTime = glfwGetTime();
    // windowData_.deltaTime = currTime - windowData_.previousTime;
    // windowData_.previousTime = currTime;
    //
    //TODO: If theres no animation left to be done, reset prevTime back to zero
    windowData_.currentEvent = HkEvent::None; /* Reset current event */

    /* Update suggested cursor accordingly. No priority is taken into account for now*/
    updateCursor();
}

void HkWindowManager::decideCursor()
{
    /* Very important to change cursor only if it might of changed*/
    if (!windowData_.cursorChangeNeeded == true) return;

    /* First cursor different than the default one will be the new cursor in this case*/
    if (potentialCursor_ == 0 && windowData_.suggestedCursor != GLFW_ARROW_CURSOR)
    {
        potentialCursor_ = windowData_.suggestedCursor;
    }
}

void HkWindowManager::updateCursor()
{
    /* Very important to change cursor only if it might of changed*/
    if (!windowData_.cursorChangeNeeded == true) return;

    /* init cursor types */
    if (!cursorsInited_)
    {
        hCursor_ = glfwCreateStandardCursor(GLFW_HRESIZE_CURSOR);
        vCursor_ = glfwCreateStandardCursor(GLFW_VRESIZE_CURSOR);
        hvCursor_ = glfwCreateStandardCursor(GLFW_CROSSHAIR_CURSOR);
        cursorsInited_ = true;
    }

    /* If we have a potential new cursor that we know is not the default one,
    change current cursor o it*/
    potentialCursor_ != 0 ?
        decidedCursor_ = potentialCursor_ : decidedCursor_ = GLFW_ARROW_CURSOR;

    switch (decidedCursor_)
    {
    case GLFW_ARROW_CURSOR:
        glfwSetCursor(windowHandle_, NULL);
        break;
    case GLFW_HRESIZE_CURSOR:
        glfwSetCursor(windowHandle_, hCursor_);
        break;
    case GLFW_VRESIZE_CURSOR:
        glfwSetCursor(windowHandle_, vCursor_);
        break;
    case GLFW_CROSSHAIR_CURSOR:
        glfwSetCursor(windowHandle_, hvCursor_);
        break;
    }

    /* Reset change needed.. already handled*/
    windowData_.cursorChangeNeeded = false;
    potentialCursor_ = 0;
}

void HkWindowManager::teardown()
{
    std::cout << "Destroying window: " << windowName_ << "\n";
    glfwDestroyCursor(hCursor_);
    glfwDestroyCursor(vCursor_);
    glfwDestroyCursor(hvCursor_);
    glfwDestroyWindow(windowHandle_);
}

void HkWindowManager::addSubWindow(const IHkRootNodePtr& subWindowRoot)
{
    subWindowRoot->getUnderlayingNode()->injectWindowDataPtr(&windowData_);
    /* Two methods of possible push:
        rootSubWindows_.push_back(subWindowRoot); // 1 2
        ^ will put new window as the front most one

        rootSubWindows_.insert(rootSubWindows_.begin(), subWindowRoot); // 2 1
        ^ will put new window as the back most one

        Code should stick to new one being the front-most one. Last element in the vector
        is always the window on top of everything
    */
    rootSubWindows_.push_back(subWindowRoot);
}

void HkWindowManager::makeContextCurrent() { glfwMakeContextCurrent(windowHandle_); }

void HkWindowManager::makeContextNotCurrent() { glfwMakeContextCurrent(NULL); }

void HkWindowManager::setBackgroundImage(const std::string& pathToImg)
{
    //TODO: this has to do with shaders and opengl, this function shall pe called on first heartbeat
    if (pathToBgImg_ != pathToImg)
    {
        std::cout << "New background image identified. Will load.\n";
        auto texInfo = windowData_.renderer.addTextureSourceToCache(pathToImg);

        /* Set TC */
        bgNodeData_.transformContext.setPos({ 0,0 });
        bgNodeData_.transformContext.setScale(windowData_.windowSize);

        /* Set RC */
        const std::string DEFAULT_VS = "assets/shaders/vTextured.glsl";
        const std::string DEFAULT_FS = "assets/shaders/fTextured.glsl";
        const HkVertexArrayType DEFAULT_TYPE = HkVertexArrayType::QUAD;
        bgNodeData_.renderContext.texInfos.push_back({ "texture1", GL_TEXTURE0, texInfo.texId });
        bgNodeData_.renderContext.shaderId = windowData_.renderer.addShaderSourceToCache(DEFAULT_VS, DEFAULT_FS);
        bgNodeData_.renderContext.vaoId = windowData_.renderer.addVertexArrayDataToCache(DEFAULT_TYPE);

        pathToBgImg_ = pathToImg;
        shouldRenderBackground_ = true;
    }
    else
    {
        std::cout << "Same background image. Will reuse.\n";
        shouldRenderBackground_ = false;
    }
}

void HkWindowManager::renderBackgroundImage()
{
    if (!shouldRenderBackground_) return;
    bgNodeData_.renderContext.windowProjMatrix = windowData_.sceneProjMatrix;
    bgNodeData_.transformContext.setScale(windowData_.windowSize + 1); //TODO: Bug similar to hkConstraint
    windowData_.renderer.render(bgNodeData_.renderContext, bgNodeData_.styleContext, bgNodeData_.transformContext.getModelMatrix());
}

void HkWindowManager::resizeEventCalled(GLFWwindow*, int width, int height)
{
    windowData_.windowSize = { width, height };
    windowData_.sceneProjMatrix = glm::ortho(0.0f, (float)width, (float)height, 0.0f, 0.0f, 100.0f);
    updateAllSubWindows(HkEvent::WindowResize);
}

void HkWindowManager::mouseMovedEventCalled(GLFWwindow*, int xPos, int yPos)
{
    windowData_.lastMousePos = windowData_.mousePos;
    windowData_.mousePos = { xPos, yPos };

    /* As mouse moves in the scene, compute first who's hovered, it might not be the 'focusedId'. Only after that,
    dispatch mouseMove event. The dragged element will become the 'focusedId'. Dragged element will be set only if
    there's not already someone being dragged */
    resolveHover();
    if (!windowData_.isDragging
        && windowData_.lastActiveMouseButton == HkMouseButton::Left
        && windowData_.hoveredId != HkWindowData::NO_SELECTION_ID
        && windowData_.isMouseClicked)
    {
        windowData_.isDragging = true;
        windowData_.dragStartMousePosition = windowData_.mousePos;
    }

    updateAllSubWindows(HkEvent::MouseMove);
}

void HkWindowManager::mouseClickedEventCalled(GLFWwindow*, int button, int action, int)
{
    //TODO: set event
    if (action == GLFW_PRESS)
    {
        windowData_.isMouseClicked = true;
        if (button == GLFW_MOUSE_BUTTON_LEFT)
        {
            windowData_.lastActiveMouseButton = HkMouseButton::Left;
        }
        else if (button == GLFW_MOUSE_BUTTON_RIGHT)
        {
            windowData_.lastActiveMouseButton = HkMouseButton::Right;
        }
        else if (button == GLFW_MOUSE_BUTTON_MIDDLE)
        {
            windowData_.lastActiveMouseButton = HkMouseButton::Middle;
        }
        else
        {
            std::cout << "Unhandled mouse button press\n";
            windowData_.lastActiveMouseButton = HkMouseButton::Unknown;
        }
        /* At this point mouse state is already applied and we want to find what's been clicked, it's ID.
        After that normal mouse click event can be dispatched, this helps isolate things even more.
        Once this function exits, 'focusedId' is guaranteed to be the currenctly selected node.
        Note: only calculate the focus at mouse click, not at release. At release the object can still be
        in focus */
        resolveFocus();
    }
    else if (action == GLFW_RELEASE)
    {
        windowData_.isMouseClicked = false;
        // windowData_.lastActiveMouseButton = HkMouseButton::None;
        windowData_.isDragging = false;
    }

    updateAllSubWindows(HkEvent::MouseClick);
}

void HkWindowManager::mouseScrollEventCalled(GLFWwindow* window, double, double yOffset)
{
    windowData_.lastScrollPosY = windowData_.scrollPosY;
    windowData_.scrollPosY = yOffset;

    updateAllSubWindows(HkEvent::HoverScan);
    updateAllSubWindows(HkEvent::MouseScroll);
}

void HkWindowManager::keyEventCalled(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    /*Set bit to true if key was pressed and reset it on key release*/
    windowData_.keyStates[key] = action == GLFW_PRESS ? 1 : 0;
    windowData_.lastKeyTriggered = key;

    /*
         00010000 -- capslock slot
         00000100 -- mods  => should eval to false
    */
    if (mods & GLFW_MOD_CAPS_LOCK)
    {
        windowData_.capsLockOn = true;
        // printf("Mods capslock exists: %x\n", mods);
    }
    else
    {
        windowData_.capsLockOn = false;
        // printf("Mods capslock NOT EXISTS: %x\n", mods);
    }

    updateAllSubWindows(HkEvent::KeyAction);
}

/* This will traverse the whole tree structure from top to bottom (back to front) and return in the
  function's variable which element is the focused one (the last one to be found on mouse's position
  after click). Same logic applies for hover. */ //TODO: maybe resolve focus/hover can be merged?
void HkWindowManager::resolveFocus()
{
    //TODO: Only resolving focus on left click might not be the best always
    if (windowData_.lastActiveMouseButton == HkMouseButton::Left)
    {
        windowData_.focusedId = HkWindowData::NO_SELECTION_ID;
        windowData_.focusedSubWindowId = HkWindowData::NO_SELECTION_ID;
        windowData_.mouseOffsetFromFocusedCenter = { 0,0 };
        updateAllSubWindows(HkEvent::FocusScan);
    }
}

void HkWindowManager::resolveHover()
{
    //TODO: Some optimization could be done here so that if
    // we are in the same hovered ID each frame, dont calculate nearest SB again
    windowData_.hoveredId = HkWindowData::NO_SELECTION_ID;
    windowData_.nearestScrollContainerId_ = HkWindowData::NO_SELECTION_ID;
    updateAllSubWindows(HkEvent::HoverScan);
}
} // hkui