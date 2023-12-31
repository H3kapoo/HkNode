#include "HkLabel.hpp"
#include <glm/gtc/matrix_transform.hpp>

#include "../APIGate/GlfwGlewGate.hpp"

namespace hkui
{
HkLabel::HkLabel(const std::string& name)
    : HkNodeBase(name, HkNodeType::Label)
{
    node_.styleContext.setColor({ 226 / 255.0f, 183 / 255.0f, 17 / 255.0f, 1.0f });
}

void HkLabel::onFirstHeartbeat()
{
    std::string DEFAULT_VS = "assets/shaders/fonts/vFont.glsl";
    std::string DEFAULT_FS = "assets/shaders/fonts/fFont.glsl";
    const HkVertexArrayType DEFAULT_TYPE = HkVertexArrayType::QUAD;

    gLTextConfig_.shaderId = windowDataPtr_->renderer.addShaderSourceToCache(DEFAULT_VS, DEFAULT_FS);
    gLTextConfig_.vaoId = windowDataPtr_->renderer.addVertexArrayDataToCache(DEFAULT_TYPE);

    fontLoader_ = windowDataPtr_->renderer.addFontLoaderSourceToCache(usrTextConfig_);
    gLTextConfig_.texId = fontLoader_->getTexId();

    gLTextConfig_.color = usrTextConfig_.getFontColor();
    std::cout << text_.size() << "\n";

    HkNodeBase::onFirstHeartbeat();
}

//TODO: To be removed. Label shall not receive user input. Testing for now
void HkLabel::onCharAction()
{
    /* Take action only if we are the currently selected id*/
    // if (windowDataPtr_->focusedId != treeStruct_.getId()) { return; }

    // char b = windowDataPtr_->pressedChar;
    // text_.push_back(b);
    // textChanged_ = true;
}

//TODO: To be removed. Label shall not receive user input. Testing for now
void HkLabel::onKeyAction()
{
    // /* Take action only if we are the currently selected id*/
    // if (windowDataPtr_->focusedId != treeStruct_.getId()) { return; }

    // uint32_t key = windowDataPtr_->lastKeyTriggered;
    // if (key == GLFW_KEY_BACKSPACE && windowDataPtr_->keyStates[key] && !text_.empty())
    // {
    //     text_.erase(text_.size() - 1);
    //     textChanged_ = true;
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

    uint32_t fontSize = usrTextConfig_.getFontSize();
    auto& labelScale = node_.transformContext.getScale();
    auto& labelPos = node_.transformContext.getPos();

    /* Resolve what data is gonna be on each text line. Do this only when scale of parent changes */
    if (labelScale.x != lastScale_.x || labelScale.y != lastScale_.y || textChanged_)
    {
        resolveDirtyText();
        textChanged_ = false;
    }

    gLTextConfig_.windowProjMatrix = windowDataPtr_->sceneProjMatrix;
    windowDataPtr_->renderer.beginTextBatch(gLTextConfig_);

    float accumulatedHeights = 0;
    int32_t spread = usrTextConfig_.getLineSpread();
    int32_t dotsCount = 0;

    /* Add each line to batch, skipping white spaces */
    for (uint32_t line = 0; line < lines_.size(); line++)
    {
        int32_t x = labelPos.x;
        int32_t y = labelPos.y + accumulatedHeights;

        const int32_t lineHeightPlusSpread = (-lines_[line].lowestPoint + lines_[line].highestPoint);
        accumulatedHeights += lineHeightPlusSpread + spread;

        if (line == lines_.size() - 1 && lines_[line].endIdx != text_.size()) { dotsCount = 3; }

        for (uint32_t i = lines_[line].startIdx; i < lines_[line].endIdx; i++)
        {
            char c;
            if (dotsCount && (i >= lines_[line].endIdx - dotsCount)) c = '.';
            else c = text_[i];

            const HkFontLoader::HkChar& ch = fontLoader_->getChar(c);
            if (c == ' ') { x += (ch.advance >> 6); continue; }

            /* Determine final position and scale*/
            float xpos = x + ch.bearing.x;
            float ypos = y - ch.bearing.y + lines_[line].highestPoint;
            float w = fontSize;
            float h = fontSize;

            int32_t toRotateAlignY = ypos - combinedCharHeights_ * 0.5f + 0.5f * h - labelPos.y;

            /* Calculate vertical text alignement*/
            int32_t pushToAlignedY = 0;
            switch (usrTextConfig_.getTextVAlign())
            {
            case HkTextUserConfig::HkTextVAlign::Top:
            {
                pushToAlignedY = std::ceil(labelPos.y + combinedCharHeights_ * 0.5f);
                toRotateAlignY = std::ceil(toRotateAlignY);
                break;
            }
            case HkTextUserConfig::HkTextVAlign::Center:
            {
                pushToAlignedY = std::ceil(labelScale.y * 0.5f + labelPos.y);
                toRotateAlignY = std::ceil(toRotateAlignY);
                break;
            }
            case HkTextUserConfig::HkTextVAlign::Bottom:
            {
                pushToAlignedY = std::ceil(labelPos.y + labelScale.y - combinedCharHeights_ * 0.5f);
                toRotateAlignY = std::ceil(toRotateAlignY - spread);
                break;
            }
            }

            /* Calculate horizontal text alignement*/
            int32_t postPushToAlignedX = 0;
            int32_t prePushToAlignedX = 0;
            switch (usrTextConfig_.getTextHAlign())
            {
            case HkTextUserConfig::HkTextHAlign::Left:
            {
                postPushToAlignedX = std::ceil(labelScale.x * 0.0f + labelPos.x);
                prePushToAlignedX = std::ceil(xpos - lines_[line].length * 0.0f + 0.5f * w - labelPos.x);
                break;
            }
            case HkTextUserConfig::HkTextHAlign::Center:
            {
                postPushToAlignedX = std::ceil(labelScale.x * 0.5f + labelPos.x);
                prePushToAlignedX = std::ceil(xpos - lines_[line].length * 0.5f + 0.5f * w - labelPos.x);
                break;
            }
            case HkTextUserConfig::HkTextHAlign::Right:
            {
                postPushToAlignedX = std::ceil(labelScale.x * 1.0f + labelPos.x);
                prePushToAlignedX = std::ceil(xpos - lines_[line].length * 1.0f + 0.5f * w - labelPos.x);
                break;
            }
            }

            /* Only add to render batch lines that are actually inside the container. If text is rotated, the rotated
            part outside of bounds will not be considered. This cutoff is purely line based. */
            const auto finalY = toRotateAlignY + pushToAlignedY;
            if ((finalY > labelPos.y + labelScale.y + lineHeightPlusSpread) || (finalY < labelPos.y))
            {
                break;
            }

            /* Construct model matrix. Read from bottom to top */
            glm::mat4 modelMat = glm::mat4(1.0f);
            modelMat = glm::translate(modelMat, glm::vec3(
                postPushToAlignedX,
                pushToAlignedY,
                -1.0f));

            if (usrTextConfig_.getTextAngle() != 0.0f)
            {
                modelMat = glm::rotate(modelMat, glm::radians(usrTextConfig_.angle), glm::vec3(0.0f, 0.0f, 1.0f));
            }

            modelMat = glm::translate(modelMat, glm::vec3(
                prePushToAlignedX,
                toRotateAlignY,
                -1.0f));
            modelMat = glm::scale(modelMat, glm::vec3(w * 1, h * 1, 1.0f));

            /* Advance and upload to render batch*/
            x += (ch.advance >> 6);
            windowDataPtr_->renderer.addToTextBatch(ch.charIndex, modelMat);
        }
    }

    /* End batch and render elements who didn't make it from last render call*/
    windowDataPtr_->renderer.endTextBatch();

    lastScale_ = labelScale;
}

void HkLabel::nextWordData(const std::string& text, uint32_t index, uint32_t& advance, float& wordLen,
    float& lowestPoint, float& highestPoint)
{
    /*Get advance needed to get to the next word, 1 space separated + also gets the wordLen in pixels */
    const auto size = text.size();
    while (index < size)
    {
        const HkFontLoader::HkChar& ch = fontLoader_->getChar(text[index + advance]);
        wordLen += (ch.advance >> 6);

        const float heightUnderBaseline = -(ch.size.y - ch.bearing.y);
        if (heightUnderBaseline < lowestPoint)
            lowestPoint = heightUnderBaseline;

        const float heightAboveBaseline = +ch.bearing.y;
        if (heightAboveBaseline > highestPoint)
            highestPoint = heightAboveBaseline;

        /* A word consista of the SPACE before it + the letters. Change 0 to 1 for that*/
        if (text[index + advance + 0] == ' ' || index + advance == size - 1)
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

    const int32_t maxRowLen = node_.transformContext.getScale().x;
    int32_t currentRowLen = 0;
    int32_t maxRowLenSoFar = 0;

    float highestPoint = -99999.0f;
    float lowestPoint = 99999.0f;

    uint32_t startLineIdx = 0;
    uint32_t lastAddedEndIndex = 0;
    uint32_t i = 0;

    combinedCharHeights_ = 0;

    const uint32_t textLen = text_.size();
    while (i < textLen)
    {
        float wordLen = 0.0f;
        uint32_t advance = 0;
        /* Decide if only full words are allowed on row or just letters from it as well*/
        if (usrTextConfig_.getWrapAtWord())
        {
            advance = 0;
            nextWordData(text_, i, advance, wordLen, lowestPoint, highestPoint);
        }
        else
        {
            advance = 1;
            const HkFontLoader::HkChar& ch = fontLoader_->getChar(text_[i]);
            wordLen = (ch.advance >> 6);

            const float heightUnderBaseline = -(ch.size.y - ch.bearing.y);
            if (heightUnderBaseline < lowestPoint)
                lowestPoint = heightUnderBaseline;

            const float heightAboveBaseline = +ch.bearing.y;
            if (heightAboveBaseline > highestPoint)
                highestPoint = heightAboveBaseline;
        }

        /* If we don't have any more available space on the current text line, save it and start a new line */
        float nextLen = currentRowLen + wordLen;
        if (nextLen > maxRowLen)
        {
            lines_.emplace_back(startLineIdx, i, currentRowLen, lowestPoint, highestPoint);
            currentRowLen = wordLen;
            startLineIdx = i;
            lastAddedEndIndex = i;

            combinedCharHeights_ += (-lowestPoint + highestPoint + usrTextConfig_.getLineSpread());

            /*Handles corner case where next line doesn't fit whole space but there's no more chars and we end up
              with line data being lp = 999  and hp = -999 which is incorrect. We shall not reset points here.*/
            if (i + advance != textLen)
            {
                lowestPoint = 9991.0f;
                highestPoint = -9999.0f;
            }

            /* Only print as many lines as user requested. Change the last 3 chars to be '...' indicating full
               text couldn't be properly fit. */
            if (lines_.size() == usrTextConfig_.getMaxRows())
            {
                printf("Max rows reached 1\n");
                const HkFontLoader::HkChar& ch = fontLoader_->getChar('.');
                const HkFontLoader::HkChar& a = fontLoader_->getChar(text_[i - 1]);
                const HkFontLoader::HkChar& b = fontLoader_->getChar(text_[i - 2]);
                const HkFontLoader::HkChar& c = fontLoader_->getChar(text_[i - 3]);
                lines_[lines_.size() - 1].length -= ((a.advance >> 6) + (b.advance >> 6) + (c.advance >> 6)); /* Subtract last 3 chars added*/
                lines_[lines_.size() - 1].length += (ch.advance >> 6) * 3; /* Add length of 3 dots */
                return;
            }
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
    if (lastAddedEndIndex != textLen)
    {
        lines_.emplace_back(lastAddedEndIndex, (uint32_t)textLen, currentRowLen, lowestPoint, highestPoint);
        combinedCharHeights_ += (-lowestPoint + highestPoint + 0);

        /* Only print as many lines as user requested. Change the last 3 chars to be '...' indicating full
           text couldn't be properly fit. */
        if (lines_.size() == usrTextConfig_.getMaxRows())
        {
            printf("Max rows reached 1\n");
            const HkFontLoader::HkChar& ch = fontLoader_->getChar('.');
            const HkFontLoader::HkChar& a = fontLoader_->getChar(text_[i - 1]);
            const HkFontLoader::HkChar& b = fontLoader_->getChar(text_[i - 2]);
            const HkFontLoader::HkChar& c = fontLoader_->getChar(text_[i - 3]);
            lines_[lines_.size() - 1].length -= ((a.advance >> 6) + (b.advance >> 6) + (c.advance >> 6)); /* Subtract last 3 chars added*/
            lines_[lines_.size() - 1].length += (ch.advance >> 6) * 3; /* Add length of 3 dots */
            return;
        }
    }

    /* Add whole string as line, it means it never got bigger than 'maxRowLen' */
    if (lines_.empty())
    {
        lines_.emplace_back(startLineIdx, (uint32_t)textLen, maxRowLenSoFar, lowestPoint, highestPoint);
        combinedCharHeights_ += (-lowestPoint + highestPoint + 0);
    }

    // printf("rowCount = %ld ;maxRowLenSoFar = %f\n", lines_.size(), maxRowLenSoFar);
    // for (uint32_t i = 0; i < lines_.size(); i++)
    // {
    //     printf("startIdx = %d ; endIdx = %d ; length = %f; low = %f; high = %f\n",
    //         lines_[i].startIdx, lines_[i].endIdx, lines_[i].length, lines_[i].lowestPoint, lines_[i].highestPoint);
    // }
}

void HkLabel::setText(const std::string& text) { text_ = text; textChanged_ = true; }

HkTextUserConfig& HkLabel::getTextStyle() { return usrTextConfig_; }
} // hkui
