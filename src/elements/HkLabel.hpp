#pragma once

#include <string>
#include <memory>
#include <map>
#include <vector>
#include <glm/glm.hpp>

#include "../base/HkNodeBase.hpp"
#include "../renderer/HkShader.hpp"
#include "../utils/HkFontLoader.hpp"

namespace hkui
{
class HkLabel : public HkNodeBase
{
public:
    HkLabel(const std::string& name);

    void setText(const std::string& text);
    HkTextUserConfig& getTextStyle();

private:
    struct HkTextLine
    {
        uint32_t startIdx{ 0 }, endIdx{ 0 };
        float length{ 0.0f };

        float lowestPoint{ 0.0f };
        float highestPoint{ 0.0f };
    };

    /* HkNodeBase */
    void onFirstHeartbeat();
    void onKeyAction();
    void postRenderAdditionalDetails();

    void resolveDirtyText();
    void nextWordData(const std::string& text, uint32_t index, uint32_t& advance, float& wordLen,
        float& lowestPoint, float& highestPoint);

    HkFontLoader* fontLoader_;

    HkTextRenderGLConfig gLTextConfig_;
    HkTextUserConfig usrTextConfig_;

    std::vector<HkTextLine> lines_;
    std::string text_;
    bool textChanged_{ false };

    glm::ivec2 lastScale_{ 1 };
    float maxLenSoFar_{ 0 };

    int32_t combinedCharHeights_{ 0 };
};

using HkLabelPtr = std::shared_ptr<HkLabel>;
using HkLabelCPtr = const std::shared_ptr<HkLabel>;
} // hkui
