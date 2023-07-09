#pragma once
#include <GLFW/glfw3.h>

#include "../base/HkNode.hpp"

namespace hkui
{

enum class HkMouseButton
{
    None,
    Left,
    Right,
    Middle
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
    double mousePosX, mousePosY;
    double lastMousePosX, lastMousePosY;
    bool isMouseClicked;
    HkMouseButton clickedHkMouseButton;
    bool mouseEntered, mouseExited;
    double scrollPosY;
    double lastScrollPosY;
    bool receivedDrop;
    int dropCount;
    const char** droppedPaths;
};

class HkSceneManagement
{
    /* Needs to be friend for it to access private sceneData */
    friend class HkButton;
    friend class HkContainer;

public:
    HkSceneManagement(const HkSceneManagement&) = delete;
    void operator=(const HkSceneManagement&) = delete;

    // To be NOTED: It's not thread safe
    static HkSceneManagement& get()
    {
        static HkSceneManagement instance;
        return instance;
    }

    void init(int wWidth, int wHeight)
    {
        sceneData.windowWidth = wWidth;
        sceneData.windowHeight = wHeight;
    }

    void setRoot(HkNodeCPtr& newRootNode)
    {
        rootNode = newRootNode;
        std::cout << "Root for scene changed to: " << rootNode << '\n';
    }

    void update(const HkEvent& ev)
    {
        sceneData.currentEvent = ev; // HkEvent gets consumed
        rootNode->updateMySelf();
        sceneData.currentEvent = HkEvent::None; // HkEvent gets consumed
    }

    void resizeWindowEvent(GLFWwindow*, int width, int height)
    {
        sceneData.windowWidth = width;
        sceneData.windowHeight = height;
        update(HkEvent::WindowResize);
    }

    void mouseMoveEvent(GLFWwindow*, double xPos, double yPos)
    {
        // std::cout << "moved to " << xPos << " " << yPos << "\n";
        sceneData.lastMousePosX = sceneData.mousePosX;
        sceneData.lastMousePosY = sceneData.mousePosY;
        sceneData.mousePosX = xPos;
        sceneData.mousePosY = yPos;
        update(HkEvent::MouseMove);
    }

    void mouseClickEvent(GLFWwindow*, int button, int action, int)
    {
        // std::cout << "mouse click ";// << xpos << " " << ypos << "\n";
        if (action == GLFW_PRESS)
        {
            sceneData.isMouseClicked = true;
            if (button == GLFW_MOUSE_BUTTON_LEFT)
            {
                sceneData.clickedHkMouseButton = HkMouseButton::Left;
            }
            else if (button == GLFW_MOUSE_BUTTON_LEFT)
            {
                sceneData.clickedHkMouseButton = HkMouseButton::Left;
            }
            else if (button == GLFW_MOUSE_BUTTON_MIDDLE)
            {
                sceneData.clickedHkMouseButton = HkMouseButton::Middle;
            }
        }
        else if (action == GLFW_RELEASE)
        {
            sceneData.isMouseClicked = false;
            sceneData.clickedHkMouseButton = HkMouseButton::None;
        }
        update(HkEvent::MouseClick);
    }

    void mouseEnterEvent(GLFWwindow*, int entered)
    {
        entered ? (sceneData.mouseEntered = true) : (sceneData.mouseEntered = false);
        // std::cout << "mouse entered or exited ";// << xpos << " " << ypos << "\n";
        update(HkEvent::MouseEnterExit);
    }

    void mouseScrollEvent(GLFWwindow*, double xOffset, double)
    {
        // std::cout << "mouse scroll\n";// << xpos << " " << ypos << "\n";
        sceneData.lastScrollPosY = sceneData.scrollPosY;
        sceneData.scrollPosY = xOffset;
        update(HkEvent::MouseScroll);
    }

    void dropEvent(GLFWwindow*, int count, const char** paths)
    {
        std::cout << "dropped something\n";// << xpos << " " << ypos << "\n";
        sceneData.receivedDrop = true;
        sceneData.dropCount = count;
        sceneData.droppedPaths = paths;
        update(HkEvent::DropPath);
    }

private:
    HkSceneManagement() = default;

    HkSceneData sceneData;
    HkNodePtr rootNode;
};

} // hkui