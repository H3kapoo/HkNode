#pragma once

#include <string>
#include <memory>
#include <vector>

#include "../APIGate/GlfwGlewGate.hpp"
#include "../base/IHkRootNode.hpp"

namespace hkui
{
class HkWindowManager
{
public:
    HkWindowManager(const std::string& windowName);

    void addSubWindow(const IHkRootNodePtr& subWindowRoot);

    void resizeEventCalled(GLFWwindow* window, int width, int height);

    inline GLFWwindow* getWindowHandle() { return windowHandle_; }
    inline uint32_t getWindowId() { return 69; }

private:
    GLFWwindow* windowHandle_;
    //TODO: Window specific params
    std::vector<IHkRootNodePtr> rootSubWindows_;
};
using HkWindowManagerPtr = std::shared_ptr<HkWindowManager>;
using HkWindowManagerCPtr = const std::shared_ptr<HkWindowManager>;
} // hkui