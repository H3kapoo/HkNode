#include "HkImageView.hpp"

namespace hkui
{
HkImageView::HkImageView(const std::string& name)
    : HkNodeBase(name, HkNodeType::ImageView)
{
    node_.renderContext.renderConfig_.colorUniformEn = false;
}

void HkImageView::onFirstHeartbeat()
{
    const std::string DEFAULT_VS = "assets/shaders/vTextured.glsl";
    const std::string DEFAULT_FS = "assets/shaders/fTextured.glsl";
    const std::string DEFAULT_TYPE = "QUAD";
    node_.renderContext.renderConfig_.shaderId = windowDataPtr_->renderer.addShaderSourceToCache(DEFAULT_VS, DEFAULT_FS);
    node_.renderContext.renderConfig_.vaoId = windowDataPtr_->renderer.addVertexArrayDataToCache("QUAD");
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