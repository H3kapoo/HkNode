#pragma once

#include <vector>

#include <glm/glm.hpp>

namespace hkui
{
enum class HkDirection
{
    Horizontal,
    Vertical
};

enum class HkVAlignment
{
    Top,
    Center,
    Bottom
};

enum class HkHAlignment
{
    Left,
    Center,
    Right
};

class HkStyleContext
{

public:
    HkStyleContext() = default;

    /* Setters */
    HkStyleContext& setColor(const glm::vec3& value);
    HkStyleContext& setRowWrapping(const bool value);
    HkStyleContext& setColWrapping(const bool value);
    HkStyleContext& setLeftMargin(const uint32_t value);
    HkStyleContext& setRightMargin(const uint32_t value);
    HkStyleContext& setTopMargin(const uint32_t value);
    HkStyleContext& setBottomMargin(const uint32_t value);
    HkStyleContext& setTopBottomMargin(const uint32_t valueT, const uint32_t valueB);
    HkStyleContext& setLeftRightMargin(const uint32_t valueL, const uint32_t valueR);
    HkStyleContext& setMargins(const uint32_t valueL, const uint32_t valueR, const uint32_t valueT, const uint32_t valueB);
    HkStyleContext& setTopBottomMargins(const uint32_t valueTB);
    HkStyleContext& setLeftRightMargins(const uint32_t valueLR);
    HkStyleContext& setAllMargins(const uint32_t valueLRTB);
    HkStyleContext& setOverflowAllowedX(const bool value);
    HkStyleContext& setOverflowAllowedY(const bool value);
    HkStyleContext& setOverflowAllowedXY(const bool value);
    HkStyleContext& setDirection(HkDirection value);
    HkStyleContext& setVAlignment(HkVAlignment value);
    HkStyleContext& setHAlignment(HkHAlignment value);


    /* Inline Getters, impl here, yes */
    inline const glm::vec3& getColor() const { return color; }
    inline bool isRowWrappingEnabled() const { return rowWrap; }
    inline bool isColWrappingEnabled() const { return colWrap; }
    inline uint32_t getLeftMargin() const { return marginLX; }
    inline uint32_t getRightMargin() const { return marginRX; }
    inline uint32_t getTopMargin() const { return marginTY; }
    inline uint32_t getBottomMargin() const { return marginBY; }
    inline bool isOverflowAllowedX() const { return overflowAllowedX; }
    inline bool isOverflowAllowedY() const { return overflowAllowedY; }
    inline HkDirection getDirection() const { return direction_; }
    inline HkVAlignment getVAlignment() const { return verticalAlignment_; }
    inline HkHAlignment getHAlignment() const { return horizontalAlignment_; }

private:
    /* Attribs */
    glm::vec3 color;
    bool rowWrap{ false };
    bool colWrap{ false };
    uint32_t marginLX{ 0 }, marginRX{ 0 };
    uint32_t marginTY{ 0 }, marginBY{ 0 };
    bool overflowAllowedX{ false }, overflowAllowedY{ false };
    HkDirection direction_{ HkDirection::Horizontal };
    HkVAlignment verticalAlignment_{ HkVAlignment::Top };
    HkHAlignment horizontalAlignment_{ HkHAlignment::Left }; //TODO: Bottom + RIght => permanent scrollbars??
};
} // hkui