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

    gLTextConfig_.shaderId = windowDataPtr_->renderer.addShaderSourceToCache(DEFAULT_VS, DEFAULT_FS);
    gLTextConfig_.vaoId = windowDataPtr_->renderer.addVertexArrayDataToCache(DEFAULT_TYPE);

    fontLoader_ = windowDataPtr_->renderer.addFontLoaderSourceToCache(usrTextConfig_);
    gLTextConfig_.texId = fontLoader_->getTexId();

    gLTextConfig_.color = usrTextConfig_.getFontColor();
    std::cout << text_.size() << "\n";
}

void HkLabel::nextWordData(const std::string& text, uint32_t index, uint32_t& advance, float& wordLen)
{
    /*Get advance needed to get to the next word, 1 space separated + also gets the wordLen in pixels */
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
    /* Start clean */
    lines_.clear();

    // const auto& end = node_.transformContext.getPos() + node_.transformContext.getScale();
    const auto& end = node_.transformContext.getScale();

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
        /* Decide if only full words are allowed on row or just letters from it as well*/
        if (usrTextConfig_.getWrapAtWord())
        {
            advance = 0;
            nextWordData(text_, i, advance, wordLen);
        }
        else
        {
            advance = 1;
            const HkFontLoader::HkChar& ch = fontLoader_->getChar(text_[i]);
            wordLen = (ch.advance >> 6) * textScale_;
        }


        /* If we don't have any more available space on the current text line, save it and start a new line */
        float nextLen = currentRowLen + wordLen;
        if (nextLen > maxRowLen)
        {
            lines_.emplace_back(startLineIdx, i, currentRowLen, currentRowWords);
            currentRowLen = wordLen;
            startLineIdx = i;
            lastAddedEndIndex = i;
        }
        /* Otherwise just keep track of how far we are so far on this text line*/
        else
        {
            currentRowLen = nextLen;
        }

        if (currentRowLen > maxRowLenSoFar)
            maxRowLenSoFar = currentRowLen;
        i += advance;
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
    maxLenSoFar_ = maxRowLenSoFar;
    // printf("rowCount = %ld ;maxRowLenSoFar = %f\n", lines_.size(), maxRowLenSoFar);
    // for (uint32_t i = 0; i < lines_.size(); i++)
    // {
    //     printf("startIdx = %d ; endIdx = %d ; length = %f; words = %d\n",
    //         lines_[i].startIdx, lines_[i].endIdx, lines_[i].length, lines_[i].wordCount);
    // }
}

void HkLabel::postRenderAdditionalDetails()
{
    /* Used instancing and methods described in: https://www.youtube.com/watch?v=S0PyZKX4lyI */

    /* Resolve options that can only be done at this stage */
    if (usrTextConfig_.getDirtyStatus())
    {
        resolveDirtyText();
        fontLoader_ = windowDataPtr_->renderer.addFontLoaderSourceToCache(usrTextConfig_);
        gLTextConfig_.texId = fontLoader_->getTexId();
        usrTextConfig_.resetDirtyStatus();
    }

    /* Resolve what data is gonna be on each text line */
    //TODO: Here we shall only resolve if parent size changed from last frame for perf reasons
    resolveDirtyText();

    gLTextConfig_.windowProjMatrix = windowDataPtr_->sceneProjMatrix;
    windowDataPtr_->renderer.beginTextBatch(gLTextConfig_);

    float fontSize = usrTextConfig_.getFontSize();
    const auto end = node_.transformContext.getScale();

    /* Add each line to batch, skipping white spaces */
    float middle = 32;
    for (uint32_t line = 0; line < lines_.size(); line++)
    {
        int32_t x = node_.transformContext.getPos().x; +end.x / 2;// +(end.x - lines_[line].length) * 0.5f;
        int32_t y = node_.transformContext.getPos().y; +fontSize * textScale_ + line * fontSize;

        for (uint32_t i = lines_[line].startIdx; i < lines_[line].endIdx; i++)
        {
            const HkFontLoader::HkChar& ch = fontLoader_->getChar(text_[i]);
            if (text_[i] == ' ') { x += (ch.advance >> 6) * textScale_; continue; }

            /* Determine final position and scale*/
            float xpos = x + ch.bearing.x * textScale_;
            float ypos = y - ch.bearing.y * textScale_;
            float w = fontSize * textScale_;
            float h = fontSize * textScale_;

            /* Construct model matrix. Read from bottom to top */
            glm::mat4 modelMat = glm::mat4(1.0f);
            modelMat = glm::translate(modelMat, glm::vec3(end.x * 0.5f, end.y * 0.5f, -1.0f));

            /* Note: Due to textures being fontSize x fontSize in dimension, FT Lib cannot cover the whole square
               area with the letter glyph, due to that, if text is rotated continously, it looks a bit off center
               but in this case it's fine since we don't plan to continously rotate it.*/
            if (usrTextConfig_.getTextAngle() != 0.0f)
            {
                modelMat = glm::rotate(modelMat, glm::radians(45.0f), glm::vec3(0.0f, 0.0f, 1.0f));
            }
            modelMat = glm::translate(modelMat, glm::vec3(xpos + w * 0.5f - lines_[line].length * 0.5f, ypos + h * 0.5f, -1.0f));
            modelMat = glm::scale(modelMat, glm::vec3(w, h, 1.0f));

            /* Advance and upload to render batch*/
            x += (ch.advance >> 6) * textScale_;
            windowDataPtr_->renderer.addToTextBatch(ch.charIndex, modelMat);
        }
    }

    /* End batch and render elements who didn't make it from last render call*/
    windowDataPtr_->renderer.endTextBatch();
}

void HkLabel::setText(const std::string& text) { text_ = text; }

HkTextUserConfig& HkLabel::getTextStyle() { return usrTextConfig_; }
} // hkui