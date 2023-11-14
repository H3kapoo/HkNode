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
        uint32_t wordCount{ 0 };

        float lowestPoint{ 0.0f };
        float highestPoint{ 0.0f };
    };

    /* HkNodeBase */
    void onFirstHeartbeat();
    void postRenderAdditionalDetails();

    void resolveDirtyText();
    void nextWordData(const std::string& text, uint32_t index, uint32_t& advance, float& wordLen, float& wordHeight);

    HkFontLoader* fontLoader_;

    HkTextRenderGLConfig gLTextConfig_;
    HkTextUserConfig usrTextConfig_;

    std::vector<HkTextLine> lines_;
    std::string text_;
    float textScale_{ 1.0f };
    float maxLenSoFar_{ 0 };

};

using HkLabelPtr = std::shared_ptr<HkLabel>;
using HkLabelCPtr = const std::shared_ptr<HkLabel>;
} // hkui
