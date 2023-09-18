#include "HkWindowManager.hpp"

#include <iostream>

namespace hkui
{
HkWindowManager::HkWindowManager(const std::string& windowName)
{
    //TODO: Ability to pass options to ctor
    windowHandle_ = glfwCreateWindow(1920, 1080, "HkUI", NULL, NULL);
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
}

void HkWindowManager::addSubWindow(const IHkRootNodePtr& subWindowRoot)
{
    rootSubWindows_.push_back(subWindowRoot);
}

void HkWindowManager::resizeEventCalled(GLFWwindow*, int, int)
{
    for (uint32_t i = 0;i < rootSubWindows_.size();i++)
    {
        rootSubWindows_[i]->rootUpdate();
    }
}
} // hkui