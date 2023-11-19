#include "HkAppManager.hpp"

#include <iostream>

#include "../utils/HkDrawDebugger.hpp"

namespace hkui
{

double HkAppManager::lastMoveTime = 0.0;
double HkAppManager::lastMoveTime2 = 0.0;
const double HkAppManager::MM_CALLBACK_THRESHOLD = 0.05; //TODO: Depending on how many items there are in the scene, this could be dynamic
const double HkAppManager::MS_CALLBACK_THRESHOLD = 0.04; // Same here

std::vector<HkWindowManagerPtr> HkAppManager::windows_ = {};
int32_t HkAppManager::resizeEventUnsolvedId_ = -1;

void HkAppManager::windowCloseCallback(GLFWwindow* window)
{
    /* Remove the closed window from the vector */
    std::cout << "A window needs to be closed..\n";
    for (auto it = windows_.begin(); it != windows_.end(); ++it)
    {
        if ((*it)->getWindowHandle() == window)
        {
            if ((*it)->isMasterWindow())
            {
                std::cout << "Found need to close window " << (*it)->getWindowName() << " is master window. Erasing all.\n";
                for (auto windowObj : windows_) { windowObj->teardown(); }
                windows_.clear();
                return;
            }

            std::cout << "Found need to close window " << (*it)->getWindowName() << " in vector of windows. Erasing.\n";
            (*it)->teardown();
            windows_.erase(it);
            return;
        }
    }
}

void HkAppManager::resizeCallback(GLFWwindow* window, int width, int height)
{
    //TODO: We shall only notify the visible/in focus window maybe
    for (uint32_t i = 0; i < windows_.size(); i++)
    {
        if (windows_[i]->getWindowHandle() == window)
        {
            resizeEventUnsolvedId_ = i;
            windows_[i]->resizeEventCalled(window, width, height);
            break;
        }
    }
}

void HkAppManager::mouseMoveCallback(GLFWwindow* window, double xpos, double ypos)
{
    double currentTime = glfwGetTime();
    if (currentTime - lastMoveTime < MM_CALLBACK_THRESHOLD)
    {
        return;
    }

    for (uint32_t i = 0; i < windows_.size(); i++)
    {
        if (windows_[i]->getWindowHandle() == window)
        {
            windows_[i]->mouseMovedEventCalled(window, xpos, ypos);
            break;
        }
    }
    lastMoveTime = currentTime;
}

void HkAppManager::mouseClickCallback(GLFWwindow* window, int button, int action, int mods)
{
    for (uint32_t i = 0; i < windows_.size(); i++)
    {
        if (windows_[i]->getWindowHandle() == window)
        {
            windows_[i]->mouseClickedEventCalled(window, button, action, mods);
            break;
        }
    }
}

void mouseEnterCallback(GLFWwindow* window, int entered)
{
    // let the user call the functions
    // HkSceneManagement::get().mouseEnterEvent(window, entered);
}

void HkAppManager::mouseScrollCallback(GLFWwindow* window, double xoffset, double yoffset)
{
    double currentTime = glfwGetTime();
    if (currentTime - lastMoveTime2 < MS_CALLBACK_THRESHOLD)
    {
        return;
    }

    for (uint32_t i = 0; i < windows_.size(); i++)
    {
        if (windows_[i]->getWindowHandle() == window)
        {
            windows_[i]->mouseScrollEventCalled(window, xoffset, yoffset);
            break;
        }
    }
    lastMoveTime2 = currentTime;
}

// void dropCallback(GLFWwindow* window, int count, const char** paths)
// {
//     // let the user call the functions
//     // HkSceneManagement::get().dropEvent(window, count, paths);
// }

void HkAppManager::keyCallback(GLFWwindow* window, int key, int scanCode, int action, int mods)
{
    for (uint32_t i = 0; i < windows_.size(); i++)
    {
        if (windows_[i]->getWindowHandle() == window)
        {
            windows_[i]->keyEventCalled(window, key, scanCode, action, mods);
            break;
        }
    }
}

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

    /* If master window is closed, all its children will be closed too */
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

            /* Needed to only resize the window that needs to be resized and not all of them*/
            if (resizeEventUnsolvedId_ == (int32_t)i)
            {
                int framebufferWidth, framebufferHeight;
                glfwGetFramebufferSize(windows_[i]->getWindowHandle(), &framebufferWidth, &framebufferHeight);
                glViewport(0, 0, framebufferWidth, framebufferHeight);
                resizeEventUnsolvedId_ = -1;
            }

            // glClearColor(1.0f, 0.0f, 0.0f, 1.0f);
            glClearColor(0.3f, 0.3f, 0.3f, 1.0f);
            glClear(GL_COLOR_BUFFER_BIT);

            windows_[i]->forceUpdate();

            /* HkDebugger, comment to make it go away :) */
            // HkDrawDebugger::get().drawBuffer();

            glfwSwapBuffers(windows_[i]->getWindowHandle());
            //TODO: waitEvents has a known bug on multiple windows such that the animations will not finish
            // and remain midair. Its possible that this happens because on window context swapping, empty
            // events are no longer redirected to the correct currently animating window.
            // One solution would be to, at runtime, in case animation is running, use only PollEvents dynamically
            // so that to unblock the next frame of animation. This will introduce a performance penalty on multiple
            // windows but I can't see another way untill windows will have a thread each.
            // Fortunatelly however, many animations should take an insignificant amount of time so maybe the penalty 
            // is not that big. Except for something like a videoPlayer that runs constantly.
            // Another idea would be if win1 runs a video for example, we switch to pollEvents, but in win2,3,etc
            // we post en empty event such that nothing happens there, just exits autmatically and buffer remains the same.
            // glfwWaitEvents(); // can be used with multiple windows. GLFW got that backed up
            glfwPollEvents();

            // Measure speed
            if (i == 0)
            {
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
}

void HkAppManager::addWindow(const HkWindowManagerPtr& window)
{
    windows_.push_back(window);
    glfwSetWindowCloseCallback(window->getWindowHandle(), windowCloseCallback);
    glfwSetFramebufferSizeCallback(window->getWindowHandle(), resizeCallback);
    glfwSetCursorPosCallback(window->getWindowHandle(), mouseMoveCallback);
    glfwSetMouseButtonCallback(window->getWindowHandle(), mouseClickCallback);
    glfwSetCursorEnterCallback(window->getWindowHandle(), mouseEnterCallback);
    glfwSetScrollCallback(window->getWindowHandle(), mouseScrollCallback);
    // glfwSetDropCallback(window->getWindowHandle(), dropCallback);
    glfwSetKeyCallback(window->getWindowHandle(), keyCallback);
    glfwSetInputMode(window->getWindowHandle(), GLFW_LOCK_KEY_MODS, GLFW_TRUE);
}

void HkAppManager::removeWindow(const HkWindowManagerPtr& window)
{
    std::cout << "A window needs to be closed on user demand..\n";
    for (auto it = windows_.begin(); it != windows_.end(); ++it)
    {
        if ((*it)->getWindowId() == window->getWindowId())
        {
            std::cout << "Found need to close window " << (*it)->getWindowName() << " in vector of windows. Erasing.\n";
            (*it)->teardown();
            windows_.erase(it);
            return;
        }
    }
}
} // hkui