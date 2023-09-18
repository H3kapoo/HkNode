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

    bool setup();
    void teardown();
    void runLoop();
    void addWindow(const HkWindowManagerPtr& window);
    void removeWindow(const HkWindowManagerPtr& window);

private:
    HkAppManager() = default;

    /* Stats related */
    double previousTime_{0};
    int frameCount_{0};
    //TODO: App specific params
    //TODO: Add who's the master window. Without it alive, app will exit even if we have other children windows active
    //TODO: Maybe for best results, each window shal run it's update loop in a separate thread and the render thread in common somehow?
    //TODO: Passing data between children windows shall be also handled somehow in the future


    uint32_t currentlyBoundContextId_{ 0 };
    static std::vector<HkWindowManagerPtr> windows_;
};
} // hkui