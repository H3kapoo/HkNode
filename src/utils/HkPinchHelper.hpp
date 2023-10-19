#pragma once

#include <glm/glm.hpp>

#include "../APIGate/GlfwGlewGate.hpp"
#include "../management/HkWindowData.hpp"

#include "../base/HkNodeData.hpp"

namespace hkui
{
class HkPinchHelper
{
public:
    HkPinchHelper() = default;

    void init(HkWindowData& windowData);
    void setGrabSize(const int32_t size);
    void onMove(HkWindowData& windowData, glm::ivec2& boundPos, glm::ivec2& boundScale);
    void onBarRender(HkWindowData& windowData, const glm::ivec2 boundPos, const glm::ivec2 boundScale);
    bool isSomethingActive();

    bool lockedInXR_{ false };
    bool lockedInXL_{ false };
    bool lockedInYT_{ false };
    bool lockedInYB_{ false };
    HkNodeData pincher_;

    static uint32_t LFilled;
    static uint32_t RFilled;
    static uint32_t TFilled;
    static uint32_t BFilled;
private:
    void cursorChange(HkWindowData& windowData, const int32_t value);

    // bool lockedInXR_{ false };
    // bool lockedInXL_{ false };
    // bool lockedInYT_{ false };
    // bool lockedInYB_{ false };
    int32_t grabSize_{ 15 };
    // HkNodeData pincher_;
};
} // hkui