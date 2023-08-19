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
    float getScrollValue() const;
    bool isScrollBarActive() const;
    void setScrollBarActive(const bool isActive);
    void setOverflowSize(int value);

    /* HkNodeBase */
    void resolveChildrenConstraints(std::vector<HkTreeStructure<HkNodeBase>*>&) override;

    void onDrag() override;
    void onClick() override;

private:
    HkKnob knob_;
    float overflowSize_;
    bool isHorizontal_;
    bool isActive_;
};
using HkScrollBarPtr = std::shared_ptr<HkScrollBar>;
using HkScrollBarCPtr = const std::shared_ptr<HkScrollBar>;
} // hkui