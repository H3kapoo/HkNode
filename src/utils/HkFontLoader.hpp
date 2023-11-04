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
        uint32_t textureId;
        long advance;
        glm::ivec2 size;
        glm::ivec2 bearing;
    };

    HkFontLoader() = default;
    bool load(const std::string& fontPath, const HkTextConfig& config);

    //TODO: This assumes char exists in map. TO be revised
    inline HkChar& getChar(char ch) { return charMap_[ch]; }
private:
    std::map<char, HkChar> charMap_;
};
} // hkui