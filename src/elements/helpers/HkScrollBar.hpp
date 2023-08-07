#pragma once

#include <string>
#include <memory>

#include "../../base/HkNodeBase.hpp"

namespace hkui
{
class HkScrollBar : public HkNodeBase
{
public:
    HkScrollBar(const std::string& name);
};
using HkScrollBarPtr = std::shared_ptr<HkScrollBar>;
using HkScrollBarCPtr = const std::shared_ptr<HkScrollBar>;
} // hkui