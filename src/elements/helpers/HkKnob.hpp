#pragma once

#include <string>
#include "../../base/HkNodeBase.hpp"

namespace hkui
{
class HkKnob : public HkNodeBase
{
public:
    HkKnob(const std::string& name, const bool isHorizontal);

    void onDrag() override;

    void setValue(float value);
    float getValue() const;
    bool isHorizontal() const;
    void computeKnobValue(const glm::ivec2 offsetFromCenter);

private:
    HkTransformContext* parentTc;
    float value_; /* From 0 to 1 */
    bool isHorizontalKnob_;
};
} // hkui