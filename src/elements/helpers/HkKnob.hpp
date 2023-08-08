#pragma once

#include <string>
#include "../../base/HkNodeBase.hpp"


namespace hkui
{
class HkKnob : public HkNodeBase
{
public:
    HkKnob(const std::string& name, const bool isHorizontal);

    void onGeneralMouseMove();

    void setValue(float value);
    float getValue() const;
    bool isHorizontal() const;
    void computeKnobValue();

private:
    float value_; /* From 0 to 1 */
    bool isHorizontalKnob_;
};
} // hkui