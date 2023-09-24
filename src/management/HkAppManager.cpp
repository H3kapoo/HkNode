#include "HkAppManager.hpp"

#include <iostream>

namespace hkui
{

double HkAppManager::lastMoveTime = 0.0;
double HkAppManager::lastMoveTime2 = 0.0;
const double HkAppManager::MM_CALLBACK_THRESHOLD = 0.05;
const double HkAppManager::MS_CALLBACK_THRESHOLD = 0.04;

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

void dropCallback(GLFWwindow* window, int count, const char** paths)
{
    // let the user call the functions
    // HkSceneManagement::get().dropEvent(window, count, paths);
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

            if (resizeEventUnsolvedId_ == (int32_t)i)
            {
                int framebufferWidth, framebufferHeight;
                glfwGetFramebufferSize(windows_[i]->getWindowHandle(), &framebufferWidth, &framebufferHeight);
                glViewport(0, 0, framebufferWidth, framebufferHeight);
                resizeEventUnsolvedId_ = -1;
            }

            glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
            glClear(GL_COLOR_BUFFER_BIT);

            windows_[i]->forceUpdate();

            glfwSwapBuffers(windows_[i]->getWindowHandle());
            // glfwWaitEvents();
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
    glfwSetDropCallback(window->getWindowHandle(), dropCallback);
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