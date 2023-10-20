#pragma once

#include <vector>
#include <glm/glm.hpp>

#include "../APIGate/GlfwGlewGate.hpp"
#include "../management/HkWindowData.hpp"

#include "../base/HkNodeData.hpp"

namespace hkui
{
class HkPinchHelper
{
public:
    struct PinchInfo
    {
        bool left{ false };
        bool right{ false };
        bool top{ false };
        bool bottom{ false };
        uint32_t nodeId{ 0 };
        uint32_t level{ 0 };
    };

    HkPinchHelper() = default;

    void init(HkWindowData& windowData);
    void setGrabSize(const int32_t size);

    void clear();
    void resolve();
    void onMouseButton(HkWindowData& windowData);
    void scan(HkWindowData& windowData, glm::ivec2& boundPos, glm::ivec2& boundScale,
        const uint32_t id, const uint32_t level);
    void onMouseMove(HkWindowData& windowData, HkNodeData& nd, HkNodeData& pnd, const uint32_t id);
    void onMove(HkWindowData& windowData, glm::ivec2& boundPos, glm::ivec2& boundScale);
    void onBarRender(HkWindowData& windowData, const glm::ivec2 boundPos, const glm::ivec2 boundScale);
    bool isSomethingActive();

    bool lockedInXR_{ false };
    bool lockedInXL_{ false };
    bool lockedInYT_{ false };
    bool lockedInYB_{ false };
    HkNodeData pincher_;

    static std::vector<PinchInfo> validityGroup_;

    // static uint32_t LFilled;
    // static uint32_t RFilled;
    // static uint32_t additionalRFilled;
    // static uint32_t TFilled;
    // static uint32_t BFilled;
private:
    void cursorChange(HkWindowData& windowData, const int32_t value);

    // bool lockedInXR_{ false };
    // bool lockedInXL_{ false };
    // bool lockedInYT_{ false };
    // bool lockedInYB_{ false };
    int32_t grabSize_{ 15 };
    // HkNodeData pincher_;
    glm::ivec2 boundPos_, boundScale_;

    static std::vector<PinchInfo> pinchInfo_;
    static bool resolved_;
};
} // hkui