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
    //TODO: set event
    updateAllSubWindows(HkEvent::None);
}

void HkWindowManager::updateAllSubWindows(const HkEvent& ev)
{
    windowData_.currentEvent = ev;
    for (uint32_t i = 0;i < rootSubWindows_.size();i++)
    {
        if (rootSubWindows_[i]->isAlive())
        {
            rootSubWindows_[i]->rootUpdate();
        }
        else
        {
            //TODO: Cleanup subWindow from vector
        }
    }
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
    rootSubWindows_.push_back(subWindowRoot);
}

void HkWindowManager::resizeEventCalled(GLFWwindow*, int width, int height)
{
    glViewport(0, 0, width, height);
    windowData_.windowSize = { width, height };
    windowData_.sceneProjMatrix = glm::ortho(0.0f, (float)width, (float)height, 0.0f, 0.0f, 100.0f);
    updateAllSubWindows(HkEvent::WindowResize);
}

void HkWindowManager::mouseMovedEventCalled(GLFWwindow*, int xPos, int yPos)
{
    //TODO: set event
    windowData_.lastMousePos = windowData_.mousePos;
    windowData_.mousePos = { xPos, yPos };

    /* As mouse moves in the scene, compute first who's hovered, it might not be the 'focusedId'. Only after that,
    dispatch mouseMove event. The dragged element will become the 'focusedId'. Dragged element will be set only if
    there's not already someone being dragged */
    resolveHover();
    if (!windowData_.isDragging && windowData_.lastActiveMouseButton == HkMouseButton::Left && windowData_.hoveredId != HkWindowData::NO_SELECTION_ID)
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
    }
    else if (action == GLFW_RELEASE)
    {
        windowData_.isMouseClicked = false;
        // windowData_.lastActiveMouseButton = HkMouseButton::None;
        windowData_.isDragging = false;
    }

    /* At this point mouse state is already applied and we want to find what's been clicked, it's ID.
    After that normal mouse click event can be dispatched, this helps isolate things even more.
    Once this function exits, 'focusedId' is guaranteed to be the currenctly selected node */
    resolveFocus();

    updateAllSubWindows(HkEvent::MouseClick);
}

void HkWindowManager::resolveFocus()
{
    //TODO: Only resolving focus on left click might not be the best always
    if (windowData_.lastActiveMouseButton == HkMouseButton::Left)
    {
        windowData_.focusedId = HkWindowData::NO_SELECTION_ID;
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