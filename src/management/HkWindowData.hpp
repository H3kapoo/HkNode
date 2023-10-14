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

    /* Events */
    HkEvent currentEvent; //G
    glm::ivec2 windowSize; //L
    glm::ivec2 mousePos; //G
    glm::ivec2 lastMousePos; //G
    bool isMouseClicked{ false }; //G
    HkMouseButton lastActiveMouseButton; //G
    bool mouseEntered, mouseExited; //L
    double scrollPosY; //L
    double lastScrollPosY; //L
    bool receivedDrop; //L
    int dropCount; //L
    const char** droppedPaths; //L

    /*Selection*/
    uint32_t focusedSubWindowId{ 0 }; //G
    uint32_t focusedId{ 0 }; //G
    glm::ivec2 mouseOffsetFromFocusedCenter{ 0,0 }; //G

    /*Drag*/
    bool isDragging{ false }; //G
    glm::ivec2 dragStartMousePosition{ 0,0 }; //G

    /*Hover*/
    uint32_t hoveredId{ 0 }; //G

    /*Rendering*/
    glm::mat4 sceneProjMatrix; //G (could be L in the future)

    /*Scene deletion*/
    bool isWindowStillAlive{ true }; //L

    /*Visibility*/
    std::unordered_map<uint32_t, bool> subWindowIdToIsMinimizedMap_; //L

    /*Scrolling*/
    uint32_t nearestScrollContainerId_{ 0 }; //G

    /*Renderer*/
    HkRenderer renderer;

    /*Animation*/
    uint32_t objectsNeedingAnimFrame{ 0 };
    double deltaTime{ 0.0f };
    double previousTime{ 0.0f };

    /*Constants*/
    static constexpr uint32_t NO_SELECTION_ID = 0;  //G
};
} // hkui