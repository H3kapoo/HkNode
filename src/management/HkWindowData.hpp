#pragma once
#include <unordered_map>
#include <glm/glm.hpp>

#include "HkEventStructs.hpp"
#include "../renderer/HkRenderer.hpp"
#include "../APIGate/GlfwGlewGate.hpp"

namespace hkui
{
//TODO: Default init each variable to avoid garbage
#define MAX_KEYS GLFW_KEY_LAST /*Max as defined in GLFW*/

struct HkWindowData
{
    /*Window obj itself*/
    GLFWwindow* windowHandle{ NULL };
    uint32_t windowId{ 0 };

    /*Events*/
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

    /*Keyboard input*/
    bool keyStates[MAX_KEYS]; /* 0 - released ; 1 - pressed*/ //TODO: Use bitfields to reduce mem to MAX_KEYS/8
    uint32_t lastKeyTriggered{ 0 };
    bool capsLockOn{ false }; /* This is the only persistent lockable state we need to cache */
    // bool numLockOn{ false }; /* Does anybody even use this? */
    uint32_t pressedChar{ 0 };

    /*Selection*/
    uint32_t focusedSubWindowId{ 0 };
    uint32_t focusedId{ 0 };
    glm::ivec2 mouseOffsetFromFocusedCenter{ 0,0 };

    /*Hover*/
    uint32_t hoveredId{ 0 };

    /*Drag*/
    bool isDragging{ false };
    glm::ivec2 dragStartMousePosition{ 0,0 };
    int32_t suggestedCursor{ 0 };
    bool cursorChangeNeeded{ false };

    /*Scrolling*/
    uint32_t nearestScrollContainerId_{ 0 };

    /*Rendering*/
    glm::mat4 sceneProjMatrix;
    HkRenderer renderer;

    /*Scene deletion*/
    bool isWindowStillAlive{ true };

    /*Visibility*/
    std::unordered_map<uint32_t, bool> subWindowIdToIsMinimizedMap_;

    /*Animation*/
    uint32_t objectsNeedingAnimFrame{ 0 };
    double deltaTime{ 0.0f };
    double previousTime{ 0.0f };

    /*Constants*/
    static constexpr uint32_t NO_SELECTION_ID = 0;
};
} // hkui