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
    // std::cout << "Focused node id is: " << sceneData.maybeFocusedNodeId << '\n';

    sceneData.currentEvent = ev; // HkEvent gets consumed
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
    // std::cout << "moved to " << xPos << " " << yPos << "\n";
    sceneData.lastMousePos = sceneData.mousePos;
    sceneData.mousePos = { xPos, yPos };
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
    update(HkEvent::MouseClick);

    /* As soon as we know who the selected node is, it becomes the focused one*/
    if (sceneData.isMouseClicked)
    {
        sceneData.maybeFocusedNodeId = sceneData.maybeSelectedNodeId;
        std::cout << "Focused id is: " << sceneData.maybeFocusedNodeId << '\n';
    }

    /* When the mouse gets released, offset should be cleared because it wouldn't
       make sense on mouseMovement to use garbage offset from previous selection*/
    if (!sceneData.isMouseClicked)
    {
        std::cout << "Maybe selected id is: " << sceneData.maybeFocusedNodeId << '\n';

        sceneData.mouseOffsetFromCenter = { 0,0 };
        sceneData.maybeSelectedNodeId = HkSceneData::NO_SELECTION_ID;
    }
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
    update(HkEvent::MouseScroll);
}

void HkSceneManagement::dropEvent(GLFWwindow*, int count, const char** paths)
{
    sceneData.receivedDrop = true;
    sceneData.dropCount = count;
    sceneData.droppedPaths = paths;
    update(HkEvent::DropPath);
}

} // hkui