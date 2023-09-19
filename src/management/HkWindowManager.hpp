#pragma once

#include <string>
#include <memory>
#include <vector>

#include "../APIGate/GlfwGlewGate.hpp"
#include "../base/IHkRootNode.hpp"

#include "HkWindowData.hpp"

namespace hkui
{
class HkWindowManager
{
    friend class HkAppManager;

public:
    struct HkWindowConfig
    {
        uint32_t width{ 800 };
        uint32_t height{ 600 };
        bool isMaster{ false };
    };

    HkWindowManager(const std::string& name, const HkWindowConfig config);

    void addSubWindow(const IHkRootNodePtr& subWindowRoot);

    inline GLFWwindow* getWindowHandle() { return windowHandle_; }
    inline uint32_t getWindowId() { return id_; }
    inline std::string getWindowName() { return windowName_; }
    inline bool isMasterWindow() { return isMaster_; }

private:
    /* Events */
    void resizeEventCalled(GLFWwindow* window, int width, int height);
    void mouseMovedEventCalled(GLFWwindow* window, int x, int y);
    void mouseClickedEventCalled(GLFWwindow* window, int button, int action, int mods);
    void mouseScrollEventCalled(GLFWwindow* window, double xOffset, double yOffset);

    void resolveFocus();
    void resolveHover();

    /* Force update from App manger */
    void forceUpdate();
    void updateAllSubWindows(const HkEvent& ev);

    /* Teardown */
    void teardown();

    //TODO: Window specific params
    HkWindowData windowData_;

    std::string windowName_;
    bool isMaster_{ false };
    uint32_t id_;
    GLFWwindow* windowHandle_;
    std::vector<IHkRootNodePtr> rootSubWindows_;

    static uint32_t idGiver;
};
using HkWindowManagerPtr = std::shared_ptr<HkWindowManager>;
using HkWindowManagerCPtr = const std::shared_ptr<HkWindowManager>;
} // hkui