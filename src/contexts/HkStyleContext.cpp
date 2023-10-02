#include "HkStyleContext.hpp"

namespace hkui
{
HkStyleContext& HkStyleContext::setColor(const glm::vec3& value)
{
    color = value;
    return *this;
}

HkStyleContext& HkStyleContext::setRowWrapping(const bool value)
{
    rowWrap = value;
    return *this;
}

HkStyleContext& HkStyleContext::setColWrapping(const bool value)
{
    colWrap = value;
    return *this;
}

HkStyleContext& HkStyleContext::setLeftMargin(const uint32_t value)
{
    marginLX = value;
    return *this;
}

HkStyleContext& HkStyleContext::setRightMargin(const uint32_t value)
{
    marginRX = value;
    return *this;
}

HkStyleContext& HkStyleContext::setTopMargin(const uint32_t value)
{
    marginTY = value;
    return *this;
}

HkStyleContext& HkStyleContext::setBottomMargin(const uint32_t value)
{
    marginBY = value;
    return *this;
}

HkStyleContext& HkStyleContext::setTopBottomMargin(const uint32_t valueT, const uint32_t valueB)
{
    setBottomMargin(valueB);
    setTopMargin(valueT);
    return *this;
}

HkStyleContext& HkStyleContext::setLeftRightMargin(const uint32_t valueL, const uint32_t valueR)
{
    setLeftMargin(valueL);
    setRightMargin(valueR);
    return *this;
}

HkStyleContext& HkStyleContext::setMargins(const uint32_t valueL, const uint32_t valueR, const uint32_t valueT,
    const uint32_t valueB)
{
    setLeftRightMargin(valueL, valueR);
    setTopBottomMargin(valueT, valueB);
    return *this;
}

HkStyleContext& HkStyleContext::setTopBottomMargins(const uint32_t valueTB)
{
    setTopBottomMargin(valueTB, valueTB);
    return *this;
}

HkStyleContext& HkStyleContext::setLeftRightMargins(const uint32_t valueLR)
{
    setLeftRightMargin(valueLR, valueLR);
    return *this;
}

HkStyleContext& HkStyleContext::setAllMargins(const uint32_t valueLRTB)
{
    setLeftRightMargins(valueLRTB);
    setTopBottomMargins(valueLRTB);
    return *this;
}

HkStyleContext& HkStyleContext::setOverflowAllowedX(const bool value)
{
    overflowAllowedX = value;
    return *this;
}

HkStyleContext& HkStyleContext::setOverflowAllowedY(const bool value)
{
    overflowAllowedY = value;
    return *this;
}

HkStyleContext& HkStyleContext::setOverflowAllowedXY(const bool value)
{
    setOverflowAllowedX(value);
    setOverflowAllowedY(value);
    return *this;
}

HkStyleContext& HkStyleContext::setDirection(HkDirection value)
{
    direction_ = value;
    return *this;
}

HkStyleContext& HkStyleContext::setVAlignment(HkVAlignment value)
{
    verticalAlignment_ = value;
    return *this;
}

HkStyleContext& HkStyleContext::setHAlignment(HkHAlignment value)
{
    horizontalAlignment_ = value;
    return *this;
}

HkStyleContext& HkStyleContext::setBackgroundImage(const std::string& value)
{
    isDirty = true;
    dirtyAttribs.insert(HkStyleDirtyAttribs::BG);
    bgImagePath_ = value;
    return *this;
}
} // hkui