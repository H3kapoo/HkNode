#pragma once

#include <glm/glm.hpp>

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

//TODO: Default init each variable to avoid garbage
struct HkSceneData
{
    HkEvent currentEvent;
    int windowWidth, windowHeight; //TODO: Deprecate
    glm::ivec2 windowSize;
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
    uint32_t focusedId{ 0 };
    glm::ivec2 mouseOffsetFromFocusedCenter{ 0,0 };

    /*Drag*/
    bool isDragging{ false };
    glm::ivec2 dragStartMousePosition{ 0,0 };

    /*Hover*/
    uint32_t hoveredId{ 0 };

    /*Rendering*/
    glm::mat4 sceneProjMatrix;

    /*Scene deletion*/ // will be more useful when multiple windowframe support comes
    bool isSceneStillAlive{ true };

    /*Visibility*/
    bool isSceneMinimized{ false };

    /*Scrolling*/
    uint32_t nearestScrollContainerId_{ 0 };

    /*Constants*/
    static constexpr uint32_t NO_SELECTION_ID = 0;
};
} // hkui