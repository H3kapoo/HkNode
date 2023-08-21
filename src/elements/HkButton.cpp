#include "HkButton.hpp"

namespace hkui
{
HkButton::HkButton(const std::string& name)
    : HkNodeBase(name, "Button")
{
    node_.renderContext.setShaderSource("assets/shaders/v1.glsl", "assets/shaders/f1.glsl");
    node_.renderContext.getShader().setVec3f("color", glm::vec3(0.761f, 0.267f, 0.78f));
    node_.renderContext.render(sceneDataRef_.sceneProjMatrix, node_.transformContext.getModelMatrix());
}

void HkButton::onClick()
{
    if (onClickCallback_ && sceneDataRef_.clickedMouseButton == HkMouseButton::Left)
    {
        onClickCallback_();
    }
}

void HkButton::setOnClickListener(std::function<void()> callback)
{
    onClickCallback_ = callback;
}
} // hkui
