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
    void setConfig(const std::string& fontPath, const HkFontLoader::HkTextConfig& config);
    void setMaxRowLen(uint32_t size) { maxRowLen_ = size; dirtyText_ = true; };

private:
    void onFirstHeartbeat();
    void postRenderAdditionalDetails();

    void renderBatch(const int32_t workingIndex);
    void resolveDirtyText();

    void nextWordData(const std::string& text, uint32_t index, uint32_t& advance, float& wordLen);

    std::string text_;
    uint32_t vaoId_, vboId_, programId_;
    HkShader shader_;
    // HkFontLoader fontLoader_;
    HkTextRenderConfig cfg_;
    HkFontLoader* fontLoader_;


    //TODO: shall be moved out in renderStore or something
    glm::mat4 tcs[400];
    int32_t letterMap[400];
    const int limit{ 400 };

    glm::vec2 textPos_{ 0,0 };

    HkFontLoader::HkTextConfig config_;

    struct HkTextLine
    {
        uint32_t startIdx{ 0 }, endIdx{ 0 };
        float length{ 0.0f };
        uint32_t wordCount{ 0 };
    };

    std::vector<HkTextLine> lines_;
    bool dirtyText_{ false };
    bool dirtyConfig_{ false };
    std::string fontPath_{ "assets/fonts/LiberationSerif-Regular.ttf" };

    float textScale_{ 1.0f };
    float maxRowLen_{ 1920.0f };
    uint32_t maxRowWords_{ 10 };
};

using HkLabelPtr = std::shared_ptr<HkLabel>;
using HkLabelCPtr = const std::shared_ptr<HkLabel>;
} // hkui
