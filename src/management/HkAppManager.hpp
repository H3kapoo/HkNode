#pragma once

#include <vector>

#include "../APIGate/GlfwGlewGate.hpp"
#include "HkWindowManager.hpp"

namespace hkui
{
class HkAppManager
{
public:
    HkAppManager(const HkAppManager&) = delete;
    void operator=(const HkAppManager&) = delete;

    /* Instance getter */
    static HkAppManager& get();

    static void windowCloseCallback(GLFWwindow* window);
    static void resizeCallback(GLFWwindow* window, int width, int height);
    static void mouseMoveCallback(GLFWwindow* window, double xpos, double ypos);
    static void mouseClickCallback(GLFWwindow* window, int button, int action, int mods);
    static void mouseScrollCallback(GLFWwindow* window, double xoffset, double yoffset);

    bool setup();
    void teardown();
    void runLoop();
    void addWindow(const HkWindowManagerPtr& window);
    void removeWindow(const HkWindowManagerPtr& window);

private:
    HkAppManager() = default;

    /* Input throtling*/
    static double lastMoveTime;
    static double lastMoveTime2;
    static const double MM_CALLBACK_THRESHOLD;
    static const double MS_CALLBACK_THRESHOLD;

    /* Stats related */
    double previousTime_{ 0 };
    int frameCount_{ 0 };
    //TODO: App specific params
    //TODO: Add who's the master window. Without it alive, app will exit even if we have other children windows active
    //TODO: Maybe for best results, each window shal run it's update loop in a separate thread and the render thread in common somehow?
    //TODO: Passing data between children windows shall be also handled somehow in the future


    uint32_t currentlyBoundContextId_{ 0 };

    static std::vector<HkWindowManagerPtr> windows_;
};
} // hkui