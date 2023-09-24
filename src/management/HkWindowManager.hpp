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
    void makeContextCurrent();
    void makeContextNotCurrent();

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
    /*
        window renderer need to know what are the currently bound objects: shader/VAO/Textures
        need to know the already generated shaders/VAOs/Textures so we dont generate them again, but reuse

        Window created
        Obj created, needs X arch and Y shader and Z texture
        Obj asks Renderer for those ids
            1. ids exist: forward them to obj, obj will have a copy of the ids
            2. ids not exist: window renderer will create and store them

        Doing it like this, each window will create that resouce once and all obj can use it.
        Storying a copy in each obj locally will ensure each frame a query doesnt happen, only on init..
        This operation shall happen on setShaderSource() this will setup everything in terms of shaders
        For VAO/ textures , a new function may be used like loadVAOForArch => this will make arches hardcoded
        which is not that bad in an UI place like this anyway. Same analogy for textures
    */
};
using HkWindowManagerPtr = std::shared_ptr<HkWindowManager>;
using HkWindowManagerCPtr = const std::shared_ptr<HkWindowManager>;
} // hkui