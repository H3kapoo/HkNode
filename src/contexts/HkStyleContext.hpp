#pragma once

#include <unordered_set>
#include <string>
#include <vector>
#include <glm/glm.hpp>

namespace hkui
{
enum class HkLayout
{
    Horizontal,
    Vertical,
    Grid
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

enum class HkStyleDirtyAttribs
{
    BG,
};

/* Vector of floats representing the percentage of the space the row/col should occupy
   Eg: {1.0, 1.0, 1.0} => all 3 rows will occpy space equally
                       => adds up to 1.0/3.0f => 0.33f => equal part
   Eg: {1.0, 0.5, 0.25} => adds up to 1/1.75f  = 0.57 => equalPart
                           1st row will get: 1.0 * 0.57 of available space => 0.58
                           2st row will get: 0.5 * 0.57 of available space => 0.29
                           3st row will get: 0.25 * 0.57 of available space => 0.145
                           = should add up to original 1.0f (obviously here's just an eg)
*/
struct HkGridConfig
{
    std::vector<float> cols{ 1.0f };
    std::vector<float> rows{ 1.0f };
};

enum class HkSizeType
{
    Absolute,
    Balanced,
    FitParent,
    FitCell,
    Percentage
};

struct HkSizeConfig
{
    HkSizeType type{ HkSizeType::Absolute };
    float value{ 100 };
    float min{ 1 };
    float max{ 1000 };
};

class HkStyleContext
{
    /* Necessary in order for base to know if to issue a dirty call to the concrete element*/
    friend class HkNodeBase;

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
    HkStyleContext& setLayout(HkLayout value);
    HkStyleContext& setVAlignment(HkVAlignment value);
    HkStyleContext& setHAlignment(HkHAlignment value);
    HkStyleContext& setVHAlignment(HkVAlignment valueV, HkHAlignment valueH);
    HkStyleContext& setGridConfig(const HkGridConfig value);
    HkStyleContext& setGridRowCol(const uint32_t valueRow, const uint32_t valueCol);
    HkStyleContext& setGridRow(const uint32_t value);
    HkStyleContext& setGridCol(const uint32_t value);
    HkStyleContext& setVSizeConfig(const HkSizeConfig& value);
    HkStyleContext& setHSizeConfig(const HkSizeConfig& value);
    HkStyleContext& setVHSizeConfig(const HkSizeConfig& valueV, const HkSizeConfig& valueH);
    HkStyleContext& setBackgroundImage(const std::string& value);


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
    inline HkLayout getLayout() const { return direction_; }
    inline HkVAlignment getVAlignment() const { return verticalAlignment_; }
    inline HkHAlignment getHAlignment() const { return horizontalAlignment_; }
    inline const HkGridConfig& getGridConfig() const { return gridConfig; };
    inline uint32_t getGridRow() const { return gridRow; };
    inline uint32_t getGridCol() const { return gridCol; };
    inline HkSizeConfig getVSizeConfig() const { return vSizeConfig; };
    inline HkSizeConfig getHSizeConfig() const { return hSizeConfig; };
    inline std::string getBackgroundImage() const { return bgImagePath_; };

private:
    /* Dirty flag */
    bool isDirty{ false };
    std::unordered_set<HkStyleDirtyAttribs> dirtyAttribs;

    /* Attribs */
    glm::vec3 color;
    bool rowWrap{ false };
    bool colWrap{ false };
    uint32_t marginLX{ 0 }, marginRX{ 0 };
    uint32_t marginTY{ 0 }, marginBY{ 0 };
    bool overflowAllowedX{ false }, overflowAllowedY{ false };
    HkLayout direction_{ HkLayout::Horizontal };
    HkVAlignment verticalAlignment_{ HkVAlignment::Top };
    HkHAlignment horizontalAlignment_{ HkHAlignment::Left }; //TODO: Bottom + RIght => permanent scrollbars??
    uint32_t gridRow{ 1 }, gridCol{ 1 };
    HkGridConfig gridConfig;
    HkSizeConfig vSizeConfig, hSizeConfig;
    std::string bgImagePath_;
};
} // hkui