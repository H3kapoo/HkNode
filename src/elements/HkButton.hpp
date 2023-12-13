#pragma once

#include <string>
#include <memory>
#include <functional>

#include "../base/HkNodeBase.hpp"
#include "HkLabel.hpp"

namespace hkui
{
class HkButton : public HkNodeBase
{
public:
    HkButton(const std::string& name);

    void setText(const std::string& text);

private:
    /* HkNodeBase */
    void onFirstHeartbeat() override;

    HkLabel textLabel_;
};

using HkButtonPtr = std::shared_ptr<HkButton>;
using HkButtonCPtr = const std::shared_ptr<HkButton>;
} // hkui
