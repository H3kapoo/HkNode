#pragma once

#include "HkEventStructs.hpp"

#include <glm/glm.hpp>

namespace hkui
{
//TODO: Default init each variable to avoid garbage
struct HkWindowData
{
    HkEvent currentEvent; //G
    glm::ivec2 windowSize; //L
    glm::ivec2 mousePos; //G
    glm::ivec2 lastMousePos; //G
    bool isMouseClicked; //G
    HkMouseButton lastActiveMouseButton; //G
    bool mouseEntered, mouseExited; //L
    double scrollPosY; //L
    double lastScrollPosY; //L
    bool receivedDrop; //L
    int dropCount; //L
    const char** droppedPaths; //L

    /*Selection*/
    uint32_t focusedId{ 0 }; //G
    glm::ivec2 mouseOffsetFromFocusedCenter{ 0,0 }; //G

    /*Drag*/
    bool isDragging{ false }; //G
    glm::ivec2 dragStartMousePosition{ 0,0 }; //G

    /*Hover*/
    uint32_t hoveredId{ 0 }; //G

    /*Rendering*/
    glm::mat4 sceneProjMatrix; //G (could be L in the future)

    /*Scene deletion*/ // will be more useful when multiple windowframe support comes
    bool isSceneStillAlive{ true }; //L

    /*Visibility*/
    bool isSceneMinimized{ false }; //L

    /*Scrolling*/
    uint32_t nearestScrollContainerId_{ 0 }; //G

    /*Constants*/
    static constexpr uint32_t NO_SELECTION_ID = 0;  //G
};
} // hkui