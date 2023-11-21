#include "HkButton.hpp"

namespace hkui
{
HkButton::HkButton(const std::string& name)
    : HkNodeBase(name, HkNodeType::Button)
{
    node_.styleContext.setColor(glm::vec4(0.761f, 0.267f, 0.78f, 1.0f));
}

void HkButton::onClick()
{
    //TODO: Remove, use events interface
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
