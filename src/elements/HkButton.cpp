#include "HkButton.hpp"

namespace hkui
{
HkButton::HkButton(const std::string& name)
    : HkNodeBase(name, HkNodeType::Button)
{
    node_.styleContext.setColor(glm::vec3(0.761f, 0.267f, 0.78f));
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
