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

    windowName_ = windowName;
    id_ = idGiver++;
    isMaster_ = config.isMaster;
}

void HkWindowManager::forceUpdate()
{
    // updateAllSubWindows(HkEvent::None);

    /* We could treat this force update as a request for an animation frame*/
    updateAllSubWindows(HkEvent::AnimationFrame);
}

void HkWindowManager::updateAllSubWindows(const HkEvent& ev)
{
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
            ++it;
        }
        else
        {
            std::cout << "SubWindow '" << (*it)->getUnderlayingNode()->getNodeInfo().name << "' requested close.\n";
            /* This will not deallocate the pointer */
            it = rootSubWindows_.erase(it);
        }
    }

    double currTime = glfwGetTime();
    windowData_.deltaTime = currTime - windowData_.previousTime;
    windowData_.previousTime = currTime;
    //TODO: If theres no animation left to be done, reset prevTime back to zero
    windowData_.currentEvent = HkEvent::None; /* Reset current event */
}

void HkWindowManager::teardown()
{
    std::cout << "Destroying window: " << windowName_ << "\n";
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