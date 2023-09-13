#pragma once

#include "../APIGate/GlfwGlewGate.hpp"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "HkSceneData.hpp"
#include "../base/IHkRootNode.hpp"

namespace hkui
{

class HkSceneManagement
{
public:
    HkSceneManagement(const HkSceneManagement&) = delete;
    void operator=(const HkSceneManagement&) = delete;

    static HkSceneManagement& get();

    HkSceneData& getSceneDataRef();

    void init(int wWidth, int wHeight);
    void setRoot(IHkRootNodeCPtr& newRootNode);
    void update();
    void resizeWindowEvent(GLFWwindow*, int width, int height);
    void mouseMoveEvent(GLFWwindow*, double xPos, double yPos);
    void mouseClickEvent(GLFWwindow*, int button, int action, int);
    void mouseEnterEvent(GLFWwindow*, int entered);
    void mouseScrollEvent(GLFWwindow*, double, double yOffset);
    void dropEvent(GLFWwindow*, int count, const char** paths);

    void resolveFocus();
    void resolveHover();

private:
    HkSceneManagement() = default;
    void update(const HkEvent& ev);

    HkSceneData sceneData;
    IHkRootNodePtr rootNode;
};

} // hkui