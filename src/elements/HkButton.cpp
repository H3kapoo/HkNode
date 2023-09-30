#include "HkButton.hpp"

namespace hkui
{
HkButton::HkButton(const std::string& name)
    : HkNodeBase(name, HkNodeType::Button)
{
    node_.styleContext.setColor(glm::vec3(0.761f, 0.267f, 0.78f));
}

void HkButton::onFirstHeartbeat()
{
    const std::string DEFAULT_VS = "assets/shaders/v1.glsl";
    const std::string DEFAULT_FS = "assets/shaders/f1.glsl";
    const std::string DEFAULT_TYPE = "QUAD";
    node_.renderContext.renderConfig_.shaderId = windowDataPtr_->renderer.addShaderSourceToCache(DEFAULT_VS, DEFAULT_FS);
    node_.renderContext.renderConfig_.vaoId = windowDataPtr_->renderer.addVertexArrayDataToCache("QUAD");
}

void HkButton::onClick()
{
    if (onClickCallback_ && windowDataPtr_->lastActiveMouseButton == HkMouseButton::Left)
    {
        onClickCallback_();
    }
}

void HkButton::setOnClickListener(std::function<void()> callback)
{
    onClickCallback_ = callback;
}
} // hkui
