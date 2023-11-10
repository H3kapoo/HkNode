#include "HkLabel.hpp"
#include "../APIGate/GlfwGlewGate.hpp"
#include <glm/gtc/matrix_transform.hpp>

namespace hkui
{
HkLabel::HkLabel(const std::string& name)
    : HkNodeBase(name, HkNodeType::Label)
{
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    node_.styleContext.setColor(glm::vec3(226 / 255.0f, 183 / 255.0f, 17 / 255.0f));
}

void HkLabel::onFirstHeartbeat()
{
    HkNodeBase::onFirstHeartbeat();

    std::string DEFAULT_VS = "assets/shaders/fonts/vFont.glsl";
    std::string DEFAULT_FS = "assets/shaders/fonts/fFont.glsl";
    const HkVertexArrayType DEFAULT_TYPE = HkVertexArrayType::QUAD;

    programId_ = shader_.loadShaderFromSource(DEFAULT_VS, DEFAULT_FS);
    vaoId_ = windowDataPtr_->renderer.addVertexArrayDataToCache(DEFAULT_TYPE);
    fontLoader_ = windowDataPtr_->renderer.addFontLoaderSourceToCache(fontPath_, config_);

    cfg_.shaderId = programId_;
    cfg_.vaoId = vaoId_;
    cfg_.texId = fontLoader_->getTexId();
    cfg_.color = glm::vec3(0.0f);
}

void HkLabel::nextWordData(const std::string& text, uint32_t index, uint32_t& advance, float& wordLen)
{
    const auto size = text.size();
    while (index < size)
    {
        const HkFontLoader::HkChar& ch = fontLoader_->getChar(text[index + advance]);
        wordLen += (ch.advance >> 6) * textScale_;
        if (text[index + advance] == ' ' || index + advance == size - 1)
        {
            advance += 1;
            break;
        }
        advance += 1;
    }
}

void HkLabel::resolveDirtyText()
{
    // if (!dirtyText_) return;

    lines_.clear();

    const auto end = node_.transformContext.getPos() + node_.transformContext.getScale();

    // const float maxRowLen = maxRowLen_;
    const float maxRowLen = end.x;
    float currentRowLen = 0;
    float maxRowLenSoFar = 0;

    uint32_t currentRowWords = 0;
    uint32_t startLineIdx = 0;
    uint32_t lastAddedEndIndex = 0;
    uint32_t i = 0;
    while (i < text_.size())
    {
        float wordLen = 0.0f;
        uint32_t advance = 0;
        // uint32_t advance = 1;

        nextWordData(text_, i, advance, wordLen);
        // const HkFontLoader::HkChar& ch = fontLoader_.getChar(text_[i]);
        // wordLen = (ch.advance >> 6) * textScale_;

        float nextLen = currentRowLen + wordLen;
        if (nextLen > maxRowLen)
        {
            lines_.emplace_back(startLineIdx, i, currentRowLen, currentRowWords);
            currentRowLen = wordLen;
            startLineIdx = i;
            lastAddedEndIndex = i;
        }
        else
        {
            currentRowLen = nextLen;
        }

        if (currentRowLen > maxRowLenSoFar)
            maxRowLenSoFar = currentRowLen;
        i += advance;
        // i += 1;

    }

    /* Add the last possible line in case it didn't make it to be bigger than 'maxRowLen' */
    if (lastAddedEndIndex != text_.size())
    {
        lines_.emplace_back(lastAddedEndIndex, (uint32_t)text_.size(), currentRowLen, currentRowWords);
    }

    /* Add whole string as line, it means it never got bigger than 'maxRowLen' */
    if (lines_.empty())
    {
        lines_.emplace_back(startLineIdx, (uint32_t)text_.size(), maxRowLenSoFar, currentRowWords);
    }

    // printf("rowCount = %ld ;maxRowLenSoFar = %f\n", lines_.size(), maxRowLenSoFar);
    // for (uint32_t i = 0; i < lines_.size(); i++)
    // {
    //     printf("startIdx = %d ; endIdx = %d ; length = %f; words = %d\n",
    //         lines_[i].startIdx, lines_[i].endIdx, lines_[i].length, lines_[i].wordCount);
    // }
    dirtyText_ = false;
}

void HkLabel::postRenderAdditionalDetails()
{
    if (dirtyConfig_)
    {
        fontLoader_ = windowDataPtr_->renderer.addFontLoaderSourceToCache(fontPath_, config_);
        cfg_.texId = fontLoader_->getTexId();
        dirtyConfig_ = false;
    }

    resolveDirtyText();
    // Use instancing and methods described in: https://www.youtube.com/watch?v=S0PyZKX4lyI
    cfg_.windowProjMatrix = windowDataPtr_->sceneProjMatrix;
    shader_.bindId(programId_);

    float factor = config_.fontSize;
    const auto end = node_.transformContext.getPos() + node_.transformContext.getScale();

    textPos_.x = 0;
    textPos_.y = 0;

    int32_t workingIndex = 0;
    for (uint32_t line = 0; line < lines_.size(); line++)
    {
        float x = textPos_.x + node_.transformContext.getPos().x;// + (end.x - lines_[line].length) * 0.5f;
        float y = textPos_.y + node_.transformContext.getPos().y + factor * textScale_ + line * factor;

        for (uint32_t i = lines_[line].startIdx; i < lines_[line].endIdx; i++)
        {
            const HkFontLoader::HkChar& ch = fontLoader_->getChar(text_[i]);
            if (text_[i] == ' ')
            {
                x += (ch.advance >> 6) * textScale_;
                continue;
            }

            float xpos = x + ch.bearing.x * textScale_;
            float ypos = y - ch.bearing.y * textScale_;
            float w = factor * textScale_;
            float h = factor * textScale_;

            glm::mat4 modelMat = glm::mat4(1.0f);
            modelMat = glm::translate(modelMat, glm::vec3(xpos + w * 0.5f, ypos + h * 0.5f, -1.0f)); // it goes negative, expected
            modelMat = glm::scale(modelMat, glm::vec3(w, h, 1.0f));

            tcs[workingIndex] = modelMat;
            letterMap[workingIndex] = ch.charIndex;

            x += (ch.advance >> 6) * textScale_;

            workingIndex++;
            if (workingIndex == limit - 1)
            {
                renderBatch(workingIndex);
                workingIndex = 0;
            }
        }
    }
    renderBatch(workingIndex);
}

void HkLabel::renderBatch(const int32_t workingIndex)
{
    if (workingIndex > 0)
    {
        cfg_.amount = workingIndex;

        int transformLoc = glGetUniformLocation(programId_, "model");
        int indexLoc = glGetUniformLocation(programId_, "letter");

        glUniformMatrix4fv(transformLoc, cfg_.amount, GL_FALSE, &tcs[0][0][0]);
        glUniform1iv(indexLoc, cfg_.amount, &letterMap[0]);
        windowDataPtr_->renderer.render(cfg_);
    }
}

void HkLabel::setConfig(const std::string& fontPath, const HkFontLoader::HkTextConfig& config)
{
    config_ = config;
    fontPath_ = fontPath;
    dirtyConfig_ = true;
}

void HkLabel::setText(const std::string& text)
{
    text_ = text;
    dirtyText_ = true;
}
} // hkui
