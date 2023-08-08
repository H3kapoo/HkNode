#pragma once

#include <string>
#include <memory>

#include "../../base/HkNodeBase.hpp"

#include "HkKnob.hpp"

namespace hkui
{

class HkScrollBar : public HkNodeBase
{
public:
    HkScrollBar(const std::string& name, const bool isHorizontal);

    bool isHorizontalScrollBar() const;
    void setScrollValue(float value);

    /* HkNodeBase */
    void resolveConstraints(std::vector<HkTreeStructure<HkNodeBase>*>&);

    void onGeneralMouseMove();
    void onGeneralMouseClick();

private:
    HkKnob knob_;
    bool isHorizontal_;
};
using HkScrollBarPtr = std::shared_ptr<HkScrollBar>;
using HkScrollBarCPtr = const std::shared_ptr<HkScrollBar>;
} // hkui