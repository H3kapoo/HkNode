#pragma once
#include <GLFW/glfw3.h>

#include "../base/HkNode.hpp"

namespace hkui
{
    enum class MouseButton
    {
        None,
        Left,
        Right,
        Middle
    };

    enum class Event
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

    struct SceneData
    {
        Event currentEvent;
        int windowWidth, windowHeight;
        double mousePosX, mousePosY;
        double lastMousePosX, lastMousePosY;
        bool isMouseClicked;
        MouseButton clickedMouseButton;
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
        static void init(int wWidth, int wHeight)
        {
            sceneData.windowWidth = wWidth;
            sceneData.windowHeight = wHeight;
        }

        static void setRoot(HkNodeCPtr& newRootNode)
        {
            rootNode = newRootNode;
            std::cout << "Root for scene changed to: " << rootNode << '\n';
        }

        // TODO: Maybe in the future add an event queue and handle each event in sequence..
        //       or maybe glfw already does that, we'll see
        static void update(const Event& ev)
        {
            std::cout << "Update loop called\n";
            sceneData.currentEvent = ev; // event gets consumed
            rootNode->updateMySelf();
            sceneData.currentEvent = Event::None; // event gets consumed
        }

        static void resizeWindowEvent(GLFWwindow*, int width, int height)
        {
            sceneData.windowWidth = width;
            sceneData.windowHeight = height;
            update(Event::WindowResize);
        }

        static void mouseMoveEvent(GLFWwindow*, double xPos, double yPos)
        {
            // std::cout << "moved to " << xPos << " " << yPos << "\n";
            sceneData.lastMousePosX = sceneData.mousePosX;
            sceneData.lastMousePosY = sceneData.mousePosY;
            sceneData.mousePosX = xPos;
            sceneData.mousePosY = yPos;
            update(Event::MouseMove);
        }

        static void mouseClickEvent(GLFWwindow*, int button, int action, int)
        {
            // std::cout << "mouse click ";// << xpos << " " << ypos << "\n";
            if (action == GLFW_PRESS)
            {
                sceneData.isMouseClicked = true;
                if (button == GLFW_MOUSE_BUTTON_LEFT)
                {
                    sceneData.clickedMouseButton = MouseButton::Left;
                }
                else if (button == GLFW_MOUSE_BUTTON_LEFT)
                {
                    sceneData.clickedMouseButton = MouseButton::Left;
                }
                else if (button == GLFW_MOUSE_BUTTON_MIDDLE)
                {
                    sceneData.clickedMouseButton = MouseButton::Middle;
                }
            }
            else if (action == GLFW_RELEASE)
            {
                sceneData.isMouseClicked = false;
                sceneData.clickedMouseButton = MouseButton::None;
            }
            update(Event::MouseClick);
        }

        static void mouseEnterEvent(GLFWwindow*, int entered)
        {
            entered ? (sceneData.mouseEntered = true) : (sceneData.mouseEntered = false);
            // std::cout << "mouse entered or exited ";// << xpos << " " << ypos << "\n";
            update(Event::MouseEnterExit);
        }

        static void mouseScrollEvent(GLFWwindow*, double xOffset, double)
        {
            // std::cout << "mouse scroll\n";// << xpos << " " << ypos << "\n";
            sceneData.lastScrollPosY = sceneData.scrollPosY;
            sceneData.scrollPosY = xOffset;
            update(Event::MouseScroll);
        }

        static void dropEvent(GLFWwindow*, int count, const char** paths)
        {
            std::cout << "dropped something\n";// << xpos << " " << ypos << "\n";
            sceneData.receivedDrop = true;
            sceneData.dropCount = count;
            sceneData.droppedPaths = paths;
            update(Event::DropPath);
        }
    private:
        static SceneData sceneData;
        static HkNodePtr rootNode;
    };
} // hkui