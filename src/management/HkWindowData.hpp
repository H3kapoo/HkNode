#pragma once
#include <unordered_map>
#include <glm/glm.hpp>

#include "HkEventStructs.hpp"
#include "../renderer/HkRenderer.hpp"
#include "../APIGate/GlfwGlewGate.hpp"

namespace hkui
{
//TODO: Default init each variable to avoid garbage
struct HkWindowData
{
    /* Window obj itself */
    GLFWwindow* windowHandle{ NULL };
    uint32_t windowId{ 0 };

    /* Events */
    HkEvent currentEvent;
    glm::ivec2 windowSize;
    glm::ivec2 mousePos;
    glm::ivec2 lastMousePos;
    bool isMouseClicked{ false };
    HkMouseButton lastActiveMouseButton;
    bool mouseEntered, mouseExited;
    double scrollPosY;
    double lastScrollPosY;
    bool receivedDrop;
    int dropCount;
    const char** droppedPaths;

    /*Selection*/
    uint32_t focusedSubWindowId{ 0 };
    uint32_t focusedId{ 0 };
    glm::ivec2 mouseOffsetFromFocusedCenter{ 0,0 };

    /*Drag*/
    bool isDragging{ false };
    glm::ivec2 dragStartMousePosition{ 0,0 };
    int32_t suggestedCursor{ 0 };
    bool cursorChangeNeeded{ false };

    /*Hover*/
    uint32_t hoveredId{ 0 };

    /*Rendering*/
    glm::mat4 sceneProjMatrix;

    /*Scene deletion*/
    bool isWindowStillAlive{ true };

    /*Visibility*/
    std::unordered_map<uint32_t, bool> subWindowIdToIsMinimizedMap_;

    /*Scrolling*/
    uint32_t nearestScrollContainerId_{ 0 };

    /*Renderer*/
    HkRenderer renderer;

    /*Animation*/
    uint32_t objectsNeedingAnimFrame{ 0 };
    double deltaTime{ 0.0f };
    double previousTime{ 0.0f };

    /*Constants*/
    static constexpr uint32_t NO_SELECTION_ID = 0;
};
} // hkui