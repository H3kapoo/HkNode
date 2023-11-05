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

    //TODO: This shall be delegated to renderStore so we dont load it each time if
    // already loaded before
    const bool success = fontLoader_.load("assets/fonts/LiberationSerif-Regular.ttf",
        HkFontLoader::HkTextConfig{
            HkFontLoader::HkTextRenderMethod::BITMAP,
            16
        });

    if (!success)
    {
        std::cerr << "Font failed to load!\n";
    }

    for (auto i = 0; i < 400;i++)
    {
        tcs[i] = glm::mat4(1.0f);
        letterMap[i] = 0;
    }
}

void HkLabel::onFirstHeartbeat()
{
    HkNodeBase::onFirstHeartbeat();

    std::string DEFAULT_VS = "assets/shaders/fonts/vFont.glsl";
    std::string DEFAULT_FS = "assets/shaders/fonts/fFont.glsl";
    const HkVertexArrayType DEFAULT_TYPE = HkVertexArrayType::QUAD;

    programId_ = shader_.loadShaderFromSource(DEFAULT_VS, DEFAULT_FS);
    vaoId_ = windowDataPtr_->renderer.addVertexArrayDataToCache(DEFAULT_TYPE);

    cfg_.shaderId = programId_;
    cfg_.vaoId = vaoId_;
    cfg_.texId = fontLoader_.getTexId();
    cfg_.color = glm::vec3(0.0f);
    std::cout << text_.size() << "\n";
    tc.setPos({ 100, 100 });
    tc.setScale({ 20, 20 });
}

void HkLabel::postRenderAdditionalDetails()
{
    //TODO: Restrict rendering zone to just locations that can be visibly seen by user
    // vPos+vScale
    // Use instancing and methods described in: https://www.youtube.com/watch?v=S0PyZKX4lyI
    cfg_.windowProjMatrix = windowDataPtr_->sceneProjMatrix;
    // cfg_.amount = 1;

    float factor = 16.0f;
    float scale = 1.0f; // 16.0f / 256.0f;

    float x = node_.transformContext.getPos().x;
    float y = node_.transformContext.getPos().y + factor * scale;

    const auto end = node_.transformContext.getPos() + node_.transformContext.getScale();
    int32_t workingIndex = 0;
    for (uint32_t i = 0; i < text_.size(); i++)
    {
        const HkFontLoader::HkChar ch = fontLoader_.getChar(text_[i]);

        if (text_[i] == ' ')
        {
            x += (ch.advance >> 6) * scale;
            continue;
        }

        float xpos = x + ch.bearing.x * scale;
        float ypos = y - ch.bearing.y * scale;

        float w = factor * scale;
        float h = factor * scale;

        if (xpos + w * 0.5f >= end.x)
        {
            x = node_.transformContext.getPos().x;
            y += factor * scale;
            xpos = x + ch.bearing.x * scale;
            ypos = y - ch.bearing.y * scale;
        }

        //TODO: Maybe this simplification could be used inside TC class also?
        glm::mat4 modelMat = glm::mat4(1.0f);
        modelMat = glm::translate(modelMat, glm::vec3(xpos + w * 0.5f, ypos + h * 0.5f, -1.0f)); // it goes negative, expected
        modelMat = glm::scale(modelMat, glm::vec3(w, h, 1.0f));

        tcs[workingIndex] = modelMat;
        letterMap[workingIndex] = ch.charIndex;

        x += (ch.advance >> 6) * scale;
        workingIndex++;

        if (workingIndex == limit - 1)
        {
            cfg_.amount = workingIndex;
            shader_.bindId(programId_);

            int transformLoc = glGetUniformLocation(programId_, "model");
            int indexLoc = glGetUniformLocation(programId_, "letter");

            glUniformMatrix4fv(transformLoc, cfg_.amount, GL_FALSE, &tcs[0][0][0]);
            glUniform1iv(indexLoc, cfg_.amount, &letterMap[0]);
            windowDataPtr_->renderer.render(cfg_, tc.getModelMatrix());
            workingIndex = 0;
        }
    }

    if (workingIndex > 0)
    {
        cfg_.amount = workingIndex;
        shader_.bindId(programId_);

        int transformLoc = glGetUniformLocation(programId_, "model");
        int indexLoc = glGetUniformLocation(programId_, "letter");

        glUniformMatrix4fv(transformLoc, cfg_.amount, GL_FALSE, &tcs[0][0][0]);
        glUniform1iv(indexLoc, cfg_.amount, &letterMap[0]);
        windowDataPtr_->renderer.render(cfg_, tc.getModelMatrix());
    }
}

void HkLabel::setText(const std::string& text)
{
    text_ = text;
}
} // hkui
