#include <string>
#include <glm/glm.hpp>
#include <map>

namespace hkui
{
class HkFontLoader
{
public:
    enum class HkTextRenderMethod
    {
        BITMAP,
        SDF
    };

    struct HkTextConfig
    {
        HkTextRenderMethod renderMethod{ 0 };
        uint32_t fontSize{ 16 };
    };

    struct HkChar
    {
        int32_t charIndex;
        long advance;
        glm::ivec2 size;
        glm::ivec2 bearing;
    };

    HkFontLoader() = default;
    bool load(const std::string& fontPath, const HkTextConfig& config);

    inline HkChar& getChar(char ch) { return charMap_[int(ch)]; }
    inline uint32_t getTexId() { return textureArrayId_; }
private:
    HkChar charMap_[128];
    uint32_t textureArrayId_{ 0 };
};
} // hkui