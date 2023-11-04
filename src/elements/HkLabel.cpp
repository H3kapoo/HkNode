#include "HkLabel.hpp"
#include "../APIGate/GlfwGlewGate.hpp"

namespace hkui
{
HkLabel::HkLabel(const std::string& name)
    : HkNodeBase(name, HkNodeType::Label)
{
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    node_.styleContext.setColor(glm::vec3(226 / 255.0f, 183 / 255.0f, 17 / 255.0f));
    const bool success = fontLoader_.load("assets/fonts/LiberationSerif-Regular.ttf",
        HkFontLoader::HkTextConfig{
            HkFontLoader::HkTextRenderMethod::BITMAP,
            16
        });

    if (!success)
    {
        std::cerr << "Font failed to load!\n";
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
}

void HkLabel::postRenderAdditionalDetails()
{
    cfg_.windowProjMatrix = windowDataPtr_->sceneProjMatrix;

    HkTransformContext tc;

    float scale = 1.0f;
    float x = node_.transformContext.getPos().x;
    float y = node_.transformContext.getPos().y + 24;

    for (auto i = text_.begin(); i != text_.end(); i++)
    {
        const HkFontLoader::HkChar& ch = fontLoader_.getChar(*i);

        float xpos = x + ch.bearing.x * scale;
        float ypos = y - ch.bearing.y * scale;

        tc.setPos({ xpos, ypos });

        float w = ch.size.x * scale;
        float h = ch.size.y * scale;
        tc.setScale({ w, h });

        cfg_.texId = ch.textureId;

        windowDataPtr_->renderer.render(cfg_, tc.getModelMatrix());
        x += (ch.advance >> 6) * scale;
    }
}

void HkLabel::setText(const std::string& text)
{
    text_ = text;
}
} // hkui
