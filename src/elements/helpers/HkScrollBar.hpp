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

    void onFirstHeartbeat() override;
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

    HkKnob knob_;
private:
    float overflowSize_;
    bool isHorizontal_;
    bool isActive_;
};
using HkScrollBarPtr = std::shared_ptr<HkScrollBar>;
using HkScrollBarCPtr = const std::shared_ptr<HkScrollBar>;
} // hkui