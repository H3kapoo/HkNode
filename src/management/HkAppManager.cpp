#include "HkAppManager.hpp"

#include <iostream>

namespace hkui
{

std::vector<HkWindowManagerPtr> HkAppManager::windows_ = {};

void HkAppManager::windowCloseCallback(GLFWwindow*) {
    // Remove the closed window from the vector
    // auto it = std::find(windows_.begin(), windows_.end(), window);
    // if (it != windows_.end())
    // {
    //     windows_.erase(it);
    // }

    //TODO: Hack for just one window for now
    windows_.clear();
}

void HkAppManager::resizeCallback(GLFWwindow* window, int width, int height)
{
    //TODO: We shall only notify the visible/in focus window maybe
    for (uint32_t i = 0; i < windows_.size(); i++)
    {
        windows_[i]->resizeEventCalled(window, width, height);
    }
}

//TODO: Input throttling
// double lastMoveTime = 0.0;
// double lastMoveTime2 = 0.0;
// // double CALLBACK_THRESHOLD = 0.3;
// double MM_CALLBACK_THRESHOLD = 0.05;
// double MS_CALLBACK_THRESHOLD = 0.04;
// void mouseMoveCallback(GLFWwindow* window, double xpos, double ypos)
// {
//     // double currentTime = glfwGetTime();

//     // if (currentTime - lastMoveTime < MM_CALLBACK_THRESHOLD)
//     // {
//     //     return;
//     // }
//     // // let the user call the functions
//     // HkSceneManagement::get().mouseMoveEvent(window, xpos, ypos);
//     // lastMoveTime = currentTime;
// }

// void mouseClickCallback(GLFWwindow* window, int button, int action, int mods)
// {
//     // let the user call the functions
//     // HkSceneManagement::get().mouseClickEvent(window, button, action, mods);
// }

// void mouseEnterCallback(GLFWwindow* window, int entered)
// {
//     // let the user call the functions
//     // HkSceneManagement::get().mouseEnterEvent(window, entered);
// }

// void mouseScrollCallback(GLFWwindow* window, double xoffset, double yoffset)
// {
//     // double currentTime = glfwGetTime();

//     // if (currentTime - lastMoveTime2 < MS_CALLBACK_THRESHOLD)
//     // {
//     //     return;
//     // }
//     // // let the user call the functions
//     // // HkSceneManagement::get().mouseScrollEvent(window, xoffset, yoffset);
//     // lastMoveTime2 = currentTime;
// }

// void dropCallback(GLFWwindow* window, int count, const char** paths)
// {
//     // let the user call the functions
//     // HkSceneManagement::get().dropEvent(window, count, paths);
// }

HkAppManager& HkAppManager::get()
{
    static HkAppManager instance;
    return instance;
}

bool HkAppManager::setup()
{
    /* Init glfw */
    if (GLFW_FALSE == glfwInit())
    {
        std::cerr << "Something happened while trying to initialize GLFW\n";
        return false;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    // glfwSwapInterval(0); // zero to disable Vsync

    /* Create mock window just to succeed initializing glew*/
    // GLFWwindow* window = glfwCreateWindow(1280, 720, "HkUI", NULL, NULL);
    glfwWindowHint(GLFW_VISIBLE, GLFW_FALSE);
    GLFWwindow* window = glfwCreateWindow(1, 1, "InitWindow", NULL, NULL);
    if (window == NULL)
    {
        std::cerr << "Failed to create glew initializing window" << std::endl;
        glfwTerminate();
        return false;
    }
    glfwMakeContextCurrent(window);

    /* Init glew after glfw (we need to have a valid context bound first) */
    if (glewInit() != GLEW_OK)
    {
        std::cerr << "GLEW failed to initialize\n";
        return false;
    }

    /* Free no longer needed init window. User must make sure now there's a context bound
       by him before using any glew/glfw API calls
    */
    glfwDestroyWindow(window);

    /* Restore future windows visibility*/
    glfwWindowHint(GLFW_VISIBLE, GLFW_TRUE);

    return true;
}

void HkAppManager::teardown()
{
    glfwTerminate();
}

void HkAppManager::runLoop()
{
    //TODO: FPS counter is not gonna work for multiple windows in thif format
    previousTime_ = glfwGetTime();

    //TODO: Shall be changed to (if master window still alive)
    while (!windows_.empty())
    {
        for (uint32_t i = 0; i < windows_.size(); i++)
        {
            /* Bind window context if not already bound*/
            if (currentlyBoundContextId_ != windows_[i]->getWindowId())
            {
                currentlyBoundContextId_ = windows_[i]->getWindowId();
                glfwMakeContextCurrent(windows_[i]->getWindowHandle());
            }

            glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

            //TODO: Refactor sceneMangement class to use this class now
            //TODO: GLFWAPI void glfwPostEmptyEvent(void); could be used in the future in case of animations
            // HkSceneManagement::get().update();
            //TODO: ^^ we might have to split rendering and updating
            // HkDrawDebugger::get().drawBuffer();
            glfwSwapBuffers(windows_[i]->getWindowHandle());
            // glfwWaitEvents();
            glfwPollEvents();

            // Measure speed
            double currentTime = glfwGetTime();
            frameCount_++;
            if (currentTime - previousTime_ >= 1.0)
            {
                glfwSetWindowTitle(windows_[i]->getWindowHandle(), std::to_string(frameCount_).c_str());
                frameCount_ = 0;
                previousTime_ = currentTime;
            }
        }

    }
}

void HkAppManager::addWindow(const HkWindowManagerPtr& window)
{
    windows_.push_back(window);
    // glfwMakeContextCurrent(window);
    // glfwSwapInterval(0); // zero to disable Vsync
    glfwSetWindowCloseCallback(window->getWindowHandle(), windowCloseCallback);
    glfwSetFramebufferSizeCallback(window->getWindowHandle(), resizeCallback);
    // glfwSetCursorPosCallback(window->getWindowHandle(), mouseMoveCallback);
    // glfwSetMouseButtonCallback(window->getWindowHandle(), mouseClickCallback);
    // glfwSetCursorEnterCallback(window->getWindowHandle(), mouseEnterCallback);
    // glfwSetScrollCallback(window->getWindowHandle(), mouseScrollCallback);
    // glfwSetDropCallback(window->getWindowHandle(), dropCallback);
}

void HkAppManager::removeWindow(const HkWindowManagerPtr& window)
{
    glfwDestroyWindow(window->getWindowHandle());
}
} // hkui