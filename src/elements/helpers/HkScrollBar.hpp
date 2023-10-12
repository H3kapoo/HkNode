#pragma once

#include <string>
#include <memory>

#include "../../base/HkNodeBase.hpp"

namespace hkui
{

class HkScrollBar : public HkNodeBase
{
public:
    HkScrollBar(const std::string& name, const bool isHorizontal);

    void onScroll() override;
    void onDrag() override;
    void onClick() override;
    void resolveChildrenConstraints(HkTreeStruct&, const HkScrollbarsSize&) override;

    /* Setters */
    void setScrollValue(float value);
    void setBarScale(uint32_t scale);
    void setScrollBarActive(const bool isActive);
    void setOverflowSize(int value);

    /* Getters */
    bool isHorizontalScrollBar() const;
    float getScrollValue() const;
    bool isScrollBarActive() const;

private:
    /* HkNodeBase */
    void onFirstHeartbeat() override;
    void onAnimationFrameRequested() override;
    void postRenderAdditionalDetails() override;

    void computeKnobValue(const glm::ivec2 offsetFromCenter);
    void setKnobValue(float value);

    HkNodeData knob_;
    uint32_t barSize_{ 20 };
    float scrollSensitivity_{ 0.05f };
    float knobValue_{ 0 }; // 0 to 1 range
    glm::ivec2 dragOffset_{ 0,0 };

    bool isAnimOngoing{ false };
    bool restarted{ false };
    float t{ 0.0f };
    float startPos, endPos;

    double startTime{ 0.0f };
    double animDuration{ 0.15f };

    float overflowSize_{ 0 };
    bool isHorizontal_{ false };
    bool isActive_{ false };
};
using HkScrollBarPtr = std::shared_ptr<HkScrollBar>;
using HkScrollBarCPtr = const std::shared_ptr<HkScrollBar>;
} // hkui