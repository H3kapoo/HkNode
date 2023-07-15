#pragma once

#include "../base/HkNode.hpp" /* Yes, order is correct due to glew/glfw3 import order */
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

namespace hkui
{

enum class HkMouseButton
{
    None,
    Left,
    Right,
    Middle,
    Unknown
};

enum class HkEvent
{
    None,
    GeneralUpdate,
    WindowResize,
    MouseMove,
    MouseClick,
    MouseEnterExit,
    MouseScroll,
    DropPath,
    // Keyboard // TO DO:
};

struct HkSceneData
{
    HkEvent currentEvent;
    int windowWidth, windowHeight;
    glm::vec2 mousePos;
    glm::vec2 lastMousePos;
    bool isMouseClicked;
    HkMouseButton clickedMouseButton;
    bool mouseEntered, mouseExited;
    double scrollPosY;
    double lastScrollPosY;
    bool receivedDrop;
    int dropCount;
    const char** droppedPaths;

    /*Selection*/
    uint32_t maybeSelectedNodeId; /* refers to the currently selected node by means of mouseClick */
    uint32_t maybeFocusedNodeId; /* refers to the node that was previously selected and its still on focus */
    glm::vec2 mouseOffsetFromCenter{ 0,0 }; /* offset from center of UI element clicked */

    /*Rendering*/
    glm::mat4 sceneProjMatrix;
};

class HkSceneManagement
{

public:
    HkSceneManagement(const HkSceneManagement&) = delete;
    void operator=(const HkSceneManagement&) = delete;

    // To be NOTED: It's not thread safe
    static HkSceneManagement& get()
    {
        static HkSceneManagement instance;
        return instance;
    }

    HkSceneData& getSceneDataRef() { return sceneData; }

    void init(int wWidth, int wHeight)
    {
        sceneData.lastMousePos = sceneData.mousePos = { 0,0 };
        sceneData.maybeSelectedNodeId = sceneData.maybeFocusedNodeId = NO_SELECTION_ID;

        sceneData.windowWidth = wWidth;
        sceneData.windowHeight = wHeight;
        sceneData.sceneProjMatrix = glm::ortho(0.0f, (float)wWidth, (float)wHeight, 0.0f, 0.0f, 100.0f);
    }

    void setRoot(HkNodeCPtr& newRootNode)
    {
        rootNode = newRootNode;
        std::cout << "Root for scene changed to: " << rootNode << '\n';
    }

    void update(const HkEvent& ev)
    {
        // std::cout << "Focused node id is: " << sceneData.maybeFocusedNodeId << '\n';

        sceneData.currentEvent = ev; // HkEvent gets consumed
        rootNode->updateMySelf();
        /* Not sure that to do with this for now, maybe it will be useful later */
        sceneData.currentEvent = HkEvent::None; // HkEvent gets consumed
    }

    void resizeWindowEvent(GLFWwindow*, int width, int height)
    {
        sceneData.windowWidth = width;
        sceneData.windowHeight = height;
        sceneData.sceneProjMatrix = glm::ortho(0.0f, (float)width, (float)height, 0.0f, 0.0f, 100.0f);
        glViewport(0, 0, width, height);

        update(HkEvent::WindowResize);
    }

    void mouseMoveEvent(GLFWwindow*, double xPos, double yPos)
    {
        // std::cout << "moved to " << xPos << " " << yPos << "\n";
        sceneData.lastMousePos = sceneData.mousePos;
        sceneData.mousePos = { xPos, yPos };
        update(HkEvent::MouseMove);
    }

    void mouseClickEvent(GLFWwindow*, int button, int action, int)
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
            std::cout << "Maybe selected id was: " << sceneData.maybeSelectedNodeId << '\n';

            sceneData.mouseOffsetFromCenter = { 0,0 };
            sceneData.maybeSelectedNodeId = NO_SELECTION_ID;
        }
    }

    void mouseEnterEvent(GLFWwindow*, int entered)
    {
        entered ? (sceneData.mouseEntered = true) : (sceneData.mouseEntered = false);
        update(HkEvent::MouseEnterExit);
    }

    void mouseScrollEvent(GLFWwindow*, double xOffset, double)
    {
        sceneData.lastScrollPosY = sceneData.scrollPosY;
        sceneData.scrollPosY = xOffset;
        update(HkEvent::MouseScroll);
    }

    void dropEvent(GLFWwindow*, int count, const char** paths)
    {
        sceneData.receivedDrop = true;
        sceneData.dropCount = count;
        sceneData.droppedPaths = paths;
        update(HkEvent::DropPath);
    }

private:
    HkSceneManagement() = default;

    const uint32_t NO_SELECTION_ID = 9999; /* Small hack to not get warnings when comparing uint to int */
    HkSceneData sceneData;
    HkNodePtr rootNode;
};

} // hkui