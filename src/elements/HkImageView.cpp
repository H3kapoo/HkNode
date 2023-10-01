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

    if (earlyImgRequest_)
    {
        auto texInfo = windowDataPtr_->renderer.addTextureSourceToCache(earlyImgPath_);
        texInfo.texName = "texture1";
        texInfo.texUnit = GL_TEXTURE0;
        node_.renderContext.texInfos.push_back(texInfo);
    }
}

void HkImageView::loadImage(const std::string& loadPath)
{
    earlyImgPath_ = loadPath;
    earlyImgRequest_ = true;
}

void HkImageView::setSize(const glm::vec2& size)
{
    node_.transformContext.setScale(size);
}
} // hkui