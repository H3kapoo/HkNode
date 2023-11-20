#pragma once

#include <glm/glm.hpp>
#include <string>

namespace hkui
{
struct HkTextRenderGLConfig
{
    glm::vec3 color{ 1.0f };
    uint32_t shaderId{ 0 };
    uint32_t vaoId{ 0 };
    uint32_t texId{ 0 };
    glm::mat4 windowProjMatrix;
};

struct HkTextRenderData
{
    static const uint32_t MAX_BATCH_SIZE{ 400 };
    int32_t letterMap[MAX_BATCH_SIZE];
    glm::mat4 modelMap[MAX_BATCH_SIZE];
};

class HkTextUserConfig
{
    friend class HkLabel;
public:
    enum class HkTextRenderMethod
    {
        BITMAP = 0,
        SDF
    };

    enum class HkTextDirection
    {
        HORIZONTAL = 0,
        VERTICAL
    };

    //TODO: Unify this with the HkAlignement from hkStyleContext
    enum class HkTextVAlign
    {
        Top,
        Center,
        Bottom
    };

    //TODO: Unify this with the HkAlignement from hkStyleContext
    enum class HkTextHAlign
    {
        Left,
        Center,
        Right
    };

    HkTextUserConfig& getTextStyle() { return *this; }
    HkTextUserConfig& setFontPath(const std::string& value) { dirtyConfig = true; fontPath = value;  return *this; }
    HkTextUserConfig& setRenderMethod(const HkTextRenderMethod value) { dirtyConfig = true; renderMethod = value;  return *this; }
    HkTextUserConfig& setFontSize(const uint32_t value) { dirtyConfig = true; fontSize = value;  return *this; }
    HkTextUserConfig& setFontColor(const glm::vec3& value) { dirtyConfig = true; color = value;  return *this; }
    HkTextUserConfig& setWrapAtWord(const bool value) { dirtyConfig = true; wrapAtWord = value;  return *this; }
    HkTextUserConfig& setTextDirection(const HkTextDirection value) { dirtyConfig = true; direction = value;  return *this; }
    HkTextUserConfig& setTextAngle(const float value) { dirtyConfig = true; angle = value;  return *this; }
    HkTextUserConfig& setTextHAlign(const HkTextHAlign value) { dirtyConfig = true; hAlign = value;  return *this; }
    HkTextUserConfig& setTextVAlign(const HkTextVAlign value) { dirtyConfig = true; vAlign = value;  return *this; }
    HkTextUserConfig& setTextVHAlign(const HkTextVAlign valueV, const HkTextHAlign valueH) { setTextVAlign(valueV); setTextHAlign(valueH);  return *this; }
    HkTextUserConfig& setLineSpread(const int32_t value) { dirtyConfig = true; lineSpread = value;  return *this; }

    inline std::string getFontPath() const { return fontPath; }
    inline HkTextRenderMethod getRenderMethod() const { return renderMethod; }
    inline uint32_t getFontSize() const { return fontSize; }
    inline glm::vec3 getFontColor() const { return color; }
    inline bool getWrapAtWord() const { return wrapAtWord; }
    inline HkTextDirection getTextDirection() const { return direction; }
    inline float getTextAngle() const { return angle; }
    inline HkTextHAlign getTextHAlign() { return hAlign; }
    inline HkTextVAlign getTextVAlign() { return vAlign; }
    inline int32_t getLineSpread() const { return lineSpread; }

private:
    void resetDirtyStatus() { dirtyConfig = false; }
    inline bool getDirtyStatus() const { return dirtyConfig; }

    std::string fontPath{ "assets/fonts/LiberationSerif-Regular.ttf" };
    HkTextRenderMethod renderMethod{ 0 };
    uint32_t fontSize{ 16 };
    glm::vec3 color{ 1.0f };
    bool wrapAtWord{ false };
    HkTextDirection direction{ 0 };
    float angle{ 0.0f };
    HkTextVAlign vAlign{ HkTextVAlign::Top };
    HkTextHAlign hAlign{ HkTextHAlign::Left };
    int32_t lineSpread{ 0 };
    bool dirtyConfig{ false };
};
} // hkui
