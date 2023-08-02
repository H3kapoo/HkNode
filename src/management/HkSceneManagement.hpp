#pragma once

#include "../base/HkNode.hpp" /* Yes, order is correct due to glew/glfw3 import order */ // TODO: fix import order, this import is no longer needed
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "../base/IHkRootNode.hpp"

namespace hkui
{
enum class HkMouseButton
{
    None,
    Left,
    Right,
    Middle,
    Unknown
};

enum class HkEvent
{
    None,
    FocusScan,
    HoverScan,
    GeneralUpdate,
    WindowResize,
    MouseMove,
    MouseClick,
    MouseEnterExit,
    MouseScroll,
    DropPath,
    // Keyboard // TO DO:
};

struct HkSceneData
{
    HkEvent currentEvent;
    int windowWidth, windowHeight;
    glm::ivec2 mousePos;
    glm::ivec2 lastMousePos;
    bool isMouseClicked;
    HkMouseButton clickedMouseButton;
    bool mouseEntered, mouseExited;
    double scrollPosY;
    double lastScrollPosY;
    bool receivedDrop;
    int dropCount;
    const char** droppedPaths;

    /*Selection*/
    uint32_t focusedId;
    glm::ivec2 mouseOffsetFromFocusedCenter;

    /*Drag*/
    bool isDragging;

    /*Hover*/
    uint32_t hoveredId;

    /*Rendering*/
    glm::mat4 sceneProjMatrix;

    /*Constants*/
    static constexpr uint32_t NO_SELECTION_ID = 0;
};

class HkSceneManagement
{
public:
    HkSceneManagement(const HkSceneManagement&) = delete;
    void operator=(const HkSceneManagement&) = delete;

    // To be NOTED: It's not thread safe
    static HkSceneManagement& get();

    HkSceneData& getSceneDataRef();

    void init(int wWidth, int wHeight);
    void setRoot(IHkRootNodeCPtr& newRootNode);
    void update();
    void resizeWindowEvent(GLFWwindow*, int width, int height);
    void mouseMoveEvent(GLFWwindow*, double xPos, double yPos);
    void mouseClickEvent(GLFWwindow*, int button, int action, int);
    void mouseEnterEvent(GLFWwindow*, int entered);
    void mouseScrollEvent(GLFWwindow*, double xOffset, double);
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