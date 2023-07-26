#pragma once

#include "../renderer/HkRenderContext.hpp"
#include "../renderer/HkTransformContext.hpp"

namespace hkui
{
class HkNodeData
{
public:
    HkRenderContext renderContext;
    HkTransformContext transformContext;
};
} // hkui