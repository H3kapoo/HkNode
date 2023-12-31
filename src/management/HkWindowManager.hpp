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

    /* Setters */
    void makeContextCurrent();
    void makeContextNotCurrent();
    void setBackgroundImage(const std::string& pathToImg);

    /* Getters */
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
    void keyEventCalled(GLFWwindow* window, int key, int scancode, int action, int mods);
    void characterEventCalled(GLFWwindow* window, unsigned int codePoint);

    void resolveFocus();
    void resolveHover();

    /* Force update from App manger */
    void forceUpdate();

    void updateAllSubWindows(const HkEvent& ev);
    void updateCursor();
    void decideCursor();

    /* Background related*/
    void renderBackgroundImage();

    /* Teardown */
    void teardown();

    HkWindowData windowData_;

    /*Background related*/
    bool shouldRenderBackground_{ false };
    std::string pathToBgImg_;
    HkNodeData bgNodeData_;

    /*Cursor info*/
    GLFWcursor* hCursor_{ NULL };
    GLFWcursor* vCursor_{ NULL };
    GLFWcursor* hvCursor_{ NULL };
    int32_t decidedCursor_{ GLFW_ARROW_CURSOR };
    int32_t potentialCursor_{ 0 };
    bool cursorsInited_{ false };
    //TODO: Window specific params
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