#pragma once

#include <string>
#include <memory>
#include <functional>

#include "../base/HkNodeBase.hpp"

namespace hkui
{
class HkButton : public HkNodeBase
{
public:
    HkButton(const std::string& name);

    void setOnClickListener(std::function<void()> callback);

private:
    void onClick() override;

    std::function<void()> onClickCallback_{ nullptr };
};

using HkButtonPtr = std::shared_ptr<HkButton>;
using HkButtonCPtr = const std::shared_ptr<HkButton>;
} // hkui
