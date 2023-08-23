#include "HkSceneManagement.hpp"

#include "../elements/HkWindowFrame.hpp"

namespace hkui
{

HkSceneManagement& HkSceneManagement::get()
{
    static HkSceneManagement instance;
    return instance;
}

HkSceneData& HkSceneManagement::getSceneDataRef() { return sceneData; }

//TODO: this function shall be deprecated
void HkSceneManagement::init(int wWidth, int wHeight)
{
    sceneData.lastMousePos = sceneData.mousePos = { 0,0 };
    sceneData.focusedId = HkSceneData::NO_SELECTION_ID;
    sceneData.hoveredId = HkSceneData::NO_SELECTION_ID;

    sceneData.windowWidth = wWidth;
    sceneData.windowHeight = wHeight;
    sceneData.windowSize = { wWidth, wHeight };
    sceneData.sceneProjMatrix = glm::ortho(0.0f, (float)wWidth, (float)wHeight, 0.0f, 0.0f, 100.0f);
}

void HkSceneManagement::setRoot(IHkRootNodeCPtr& newRootNode)
{
    rootNode = newRootNode;
    sceneData.isSceneStillAlive = true;
    HkWindowFrameCPtr dcRoot = std::dynamic_pointer_cast<HkWindowFrame>(newRootNode);
    if (dcRoot)
    {
        std::cout << "Root for scene changed to: " << dcRoot->treeStruct_.getName() << '\n';
    }
    else
    {
        std::cout << "Something happened during root downcasting. Worth a check\n";
    }
}

//TODO: Investigate in the future why calling update() one time generates more than one event loop
void HkSceneManagement::update()
{
    update(HkEvent::GeneralUpdate);
}

void HkSceneManagement::update(const HkEvent& ev)
{
    /* Not consider the setted root as root anymore. Kind of emulate its deletion*/
    if (!sceneData.isSceneStillAlive)
    {
        rootNode = nullptr;
        return;
    }

    sceneData.currentEvent = ev;
    rootNode->rootUpdateMySelf();
    /* Not sure that to do with this for now, maybe it will be useful later */
    sceneData.currentEvent = HkEvent::None; // HkEvent gets consumed

}

void HkSceneManagement::resizeWindowEvent(GLFWwindow*, int width, int height)
{
    sceneData.windowSize = { width, height };
    sceneData.windowWidth = width;
    sceneData.windowHeight = height;
    sceneData.sceneProjMatrix = glm::ortho(0.0f, (float)width, (float)height, 0.0f, 0.0f, 100.0f);
    glViewport(0, 0, width, height);

    update(HkEvent::WindowResize);
}

void HkSceneManagement::mouseMoveEvent(GLFWwindow*, double xPos, double yPos)
{
    sceneData.lastMousePos = sceneData.mousePos;
    sceneData.mousePos = { xPos, yPos };

    /* As mouse moves in the scene, compute first who's hovered, it might not be the 'focusedId'. Only after that,
    dispatch mouseMove event. The dragged element will become the 'focusedId'. Dragged element will be set only if
    there's not already someone being dragged */
    resolveHover();
    if (!sceneData.isDragging && sceneData.clickedMouseButton == HkMouseButton::Left && sceneData.hoveredId != HkSceneData::NO_SELECTION_ID)
    {
        sceneData.isDragging = true;
        sceneData.dragStartMousePosition = sceneData.mousePos;
    }

    // std::cout << glfwGetTime() << " Hovered: " << sceneData.hoveredId << " is dragging id: " << sceneData.focusedId << "\n";

    update(HkEvent::MouseMove);
}

void HkSceneManagement::mouseClickEvent(GLFWwindow*, int button, int action, int)
{
    if (action == GLFW_PRESS)
    {
        sceneData.isMouseClicked = true;
        if (button == GLFW_MOUSE_BUTTON_LEFT)
        {
            sceneData.clickedMouseButton = HkMouseButton::Left;
        }
        else if (button == GLFW_MOUSE_BUTTON_RIGHT)
        {
            sceneData.clickedMouseButton = HkMouseButton::Right;
        }
        else if (button == GLFW_MOUSE_BUTTON_MIDDLE)
        {
            sceneData.clickedMouseButton = HkMouseButton::Middle;
        }
        else
        {
            std::cout << "Unhandled mouse button press\n";
            sceneData.clickedMouseButton = HkMouseButton::Unknown;
        }
    }
    else if (action == GLFW_RELEASE)
    {
        sceneData.isMouseClicked = false;
        sceneData.clickedMouseButton = HkMouseButton::None;

        sceneData.isDragging = false;
    }

    /* At this point mouse state is already applied and we want to find what's been clicked, it's ID.
    After that normal mouse click event can be dispatched, this helps isolate things even more.
    Once this function exits, 'focusedId' is guaranteed to be the currenctly selected node */
    resolveFocus();

    //TODO: Selection is also broken when talking about scissors overflow.. maybe we need to also compute
    //      what part of the UI Element is actually visible
    update(HkEvent::MouseClick);
}

void HkSceneManagement::mouseEnterEvent(GLFWwindow*, int entered)
{
    entered ? (sceneData.mouseEntered = true) : (sceneData.mouseEntered = false);
    update(HkEvent::MouseEnterExit);
}

void HkSceneManagement::mouseScrollEvent(GLFWwindow*, double, double yOffset)
{
    sceneData.lastScrollPosY = sceneData.scrollPosY;
    sceneData.scrollPosY = yOffset;

    /*TODO: Here we should resolve both focus and hover in the future. Think of recycle view */
    update(HkEvent::MouseScroll);
}

void HkSceneManagement::dropEvent(GLFWwindow*, int count, const char** paths)
{
    sceneData.receivedDrop = true;
    sceneData.dropCount = count;
    sceneData.droppedPaths = paths;
    update(HkEvent::DropPath);
}

void HkSceneManagement::resolveFocus()
{
    if (sceneData.clickedMouseButton == HkMouseButton::Left)
    {
        sceneData.focusedId = HkSceneData::NO_SELECTION_ID;
        sceneData.mouseOffsetFromFocusedCenter = { 0,0 };
        update(HkEvent::FocusScan);
    }
}

void HkSceneManagement::resolveHover()
{
    //TODO: Some optimization could be done here so that if
    // we are in the same hovered ID each frame, dont calculate nearest SB again
    sceneData.hoveredId = HkSceneData::NO_SELECTION_ID;
    sceneData.nearestScrollContainerId_ = HkSceneData::NO_SELECTION_ID;
    update(HkEvent::HoverScan);
}
} // hkui
