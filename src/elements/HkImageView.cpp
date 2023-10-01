#include "HkImageView.hpp"

namespace hkui
{
HkImageView::HkImageView(const std::string& name)
    : HkNodeBase(name, HkNodeType::ImageView)
{
    node_.renderContext.colorUniformEn = false;
}

void HkImageView::onFirstHeartbeat()
{
    const std::string DEFAULT_VS = "assets/shaders/vTextured.glsl";
    const std::string DEFAULT_FS = "assets/shaders/fTextured.glsl";
    const HkVertexArrayType DEFAULT_TYPE = HkVertexArrayType::QUAD;
    node_.renderContext.shaderId = windowDataPtr_->renderer.addShaderSourceToCache(DEFAULT_VS, DEFAULT_FS);
    node_.renderContext.vaoId = windowDataPtr_->renderer.addVertexArrayDataToCache(DEFAULT_TYPE);
}

void HkImageView::loadImage(const std::string& loadPath)
{
    if (texture_.loadTexture(loadPath))
    {
        node_.renderContext.texInfos.push_back({ "texture1", GL_TEXTURE0, texture_.getTextureId() });
    }
}

void HkImageView::setSize(const glm::vec2& size)
{
    node_.transformContext.setScale(size);
}
} // hkui