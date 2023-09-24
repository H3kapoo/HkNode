#pragma once

#include <string>
#include "../../base/HkNodeBase.hpp"

namespace hkui
{
class HkKnob : public HkNodeBase
{
public:
    HkKnob(const std::string& name, const bool isHorizontal);

    void onFirstHeartbeat() override;
    void onDrag() override;
    void onScroll() override;
    void scrollOngoing();
    void computeKnobValue(const glm::ivec2 offsetFromCenter);

    /* Setters */
    void setValue(float value);

    /* Getters */
    float getValue() const;
    bool isHorizontal() const;


private:
    float value_; /* From 0 to 1 */
    bool isHorizontalKnob_;
    float scrollSensitivity_;
};
} // hkui