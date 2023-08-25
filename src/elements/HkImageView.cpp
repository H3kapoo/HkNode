#include "HkImageView.hpp"

namespace hkui
{
HkImageView::HkImageView(const std::string& name)
    : HkNodeBase(name, "ImageView")
{
    node_.renderContext.setShaderSource("assets/shaders/vTextured.glsl", "assets/shaders/fTextured.glsl");
    // node_.renderContext.getShader().setVec3f("color", glm::vec3(0.761f, 0.267f, 0.78f));
    node_.renderContext.render(sceneDataRef_.sceneProjMatrix, node_.transformContext.getModelMatrix());
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