#pragma once
#include <GLFW/glfw3.h>

#include "../base/HkNode.hpp"

namespace hkui
{
    class HkSceneManagement
    {
        struct SceneData
        {
            int windowWidth, windowHeight;
        };
    public:
        static void setRoot(HkNodeCPtr& newRootNode)
        {
            rootNode = newRootNode;
            std::cout << "Root for scene changed to: " << rootNode << '\n';
        }

        static void update()
        {
            std::cout << "Update loop called\n";
            rootNode->updateMySelf();
        }

        static void resizeEvent(GLFWwindow*, int width, int height)
        {
            std::cout << "resized to " << width << " " << height << "\n";
            // update();
        }


    private:
        static SceneData sceneData;
        static HkNodePtr rootNode;
    };
} // hkui