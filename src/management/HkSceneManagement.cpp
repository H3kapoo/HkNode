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

void HkSceneManagement::init(int wWidth, int wHeight)
{
    sceneData.lastMousePos = sceneData.mousePos = { 0,0 };
    sceneData.maybeSelectedNodeId = sceneData.maybeFocusedNodeId = HkSceneData::NO_SELECTION_ID;

    sceneData.windowWidth = wWidth;
    sceneData.windowHeight = wHeight;
    sceneData.sceneProjMatrix = glm::ortho(0.0f, (float)wWidth, (float)wHeight, 0.0f, 0.0f, 100.0f);
}

void HkSceneManagement::setRoot(IHkRootNodeCPtr& newRootNode)
{
    rootNode = newRootNode;
    HkWindowFrameCPtr downcastedTemp = std::dynamic_pointer_cast<HkWindowFrame>(newRootNode);
    if (downcastedTemp)
    {
        std::cout << "Root for scene changed to: " << "TODO: Help print this node" << '\n';
    }
    else
    {
        std::cout << "Something happened during root downcasting. Worth a check\n";
    }
}

void HkSceneManagement::update(const HkEvent& ev)
{
    sceneData.currentEvent = ev;
    rootNode->rootUpdateMySelf();
    /* Not sure that to do with this for now, maybe it will be useful later */
    sceneData.currentEvent = HkEvent::None; // HkEvent gets consumed
}

void HkSceneManagement::resizeWindowEvent(GLFWwindow*, int width, int height)
{
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

    resolveHover();
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
    }

    /* At this point mouse state is already applied and we want to find what's been clicked, it's ID.
    After that normal mouse click event can be dispatched, this helps isolate things even more.
    Once this function exits, 'maybeSelectedNode' is guaranteed to be the currenctly selected node..or hovered,
    or whatever you like. */
    resolveFocus();
    update(HkEvent::MouseClick);
}

void HkSceneManagement::mouseEnterEvent(GLFWwindow*, int entered)
{
    entered ? (sceneData.mouseEntered = true) : (sceneData.mouseEntered = false);
    update(HkEvent::MouseEnterExit);
}

void HkSceneManagement::mouseScrollEvent(GLFWwindow*, double xOffset, double)
{
    sceneData.lastScrollPosY = sceneData.scrollPosY;
    sceneData.scrollPosY = xOffset;

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
    /*Aux focus id shall only be cleared when we really have the potential for focused ID to change,
    aka when we click with the left mouse button */
    if (sceneData.isMouseClicked && sceneData.clickedMouseButton == HkMouseButton::Left)
    {
        sceneData.focusedIdAux = HkSceneData::NO_SELECTION_ID;
        sceneData.mouseOffsetFromFocusedCenter = { 0,0 };
    }

    update(HkEvent::FocusHoverScan);

    /* This handles so that on mouse release, selection from mouse click is still available */
    if (sceneData.focusedIdAux != sceneData.focusedId)
        sceneData.focusedId = sceneData.focusedIdAux;

    // std::cout << glfwGetTime() << " Focused ID is: " << sceneData.focusedId << '\n';
}

void HkSceneManagement::resolveHover()
{
    sceneData.hoveredId = HkSceneData::NO_SELECTION_ID;
    update(HkEvent::FocusHoverScan);

    std::cout << glfwGetTime() << " Hovered ID is: " << sceneData.hoveredId << '\n';
}
} // hkui