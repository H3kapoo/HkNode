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

    void enableOrDisableGrabPoints(HkWindowData& windowData, std::vector<HkNodeBase*> children);

    /* Functions, in rough order of their calling*/
    void scan(HkWindowData& windowData, HkNodeData& nd, const uint32_t id, const uint32_t level);
    void onMouseButton(HkWindowData& windowData);
    void resolve();
    void onMouseMove(HkWindowData& windowData, HkNodeData& nd, HkNodeData& pnd, const uint32_t id);
    void onBarRender(HkWindowData& windowData, const glm::ivec2 boundPos, const glm::ivec2 boundScale);
    void clear();

    /* Special for window frame*/
    void scanCustom(HkWindowData& windowData, glm::ivec2& boundPos, glm::ivec2& boundScale);
    bool onMouseMoveCustom(HkWindowData& windowData, glm::ivec2& boundPos, glm::ivec2& boundScale);

    /* Setters */
    void setGrabSize(const int32_t size);
    void setGrabConfig(const HkPinchConfig& config);

    /* Getters */
    bool isEnabled();

private:
    void cursorChange(HkWindowData& windowData, const int32_t value);

    bool lockedInXR_{ false };
    bool lockedInXL_{ false };
    bool lockedInYT_{ false };
    bool lockedInYB_{ false };
    bool allowXL_{ false }, allowXR_{ false };
    bool allowYB_{ false }, allowYT_{ false };

    bool freezeXL_{ false }, freezeXR_{ false };
    bool freezeYB_{ false }, freezeYT_{ false };

    bool enabled_{ false };
    int32_t grabSize_{ 15 };
    HkNodeData pincher_;

    /* Statics */
    static std::vector<PinchInfo> pinchInfo_;
    static std::vector<PinchInfo> validityGroup_;
    static bool resolved_;
};
} // hkui