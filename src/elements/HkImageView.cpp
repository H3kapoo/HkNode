#include "HkImageView.hpp"

namespace hkui
{
HkImageView::HkImageView(const std::string& name)
    : HkNodeBase(name, HkNodeType::ImageView)
{
    node_.renderContext.setColorUniformEnabled(false);
}

void HkImageView::onFirstHeartbeat()
{
    node_.renderContext.setShaderSource("assets/shaders/vTextured.glsl", "assets/shaders/fTextured.glsl", &windowDataPtr_->renderStore);
}

void HkImageView::loadImage(const std::string& loadPath)
{
    if (textureContext_.loadTexture(loadPath))
    {
        node_.renderContext.addTexture({ "texture1", GL_TEXTURE0, textureContext_.getTexture().getTextureId() });
    }
}

void HkImageView::setSize(const glm::vec2& size)
{
    node_.transformContext.setScale(size);
}
} // hkui