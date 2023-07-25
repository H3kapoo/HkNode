#pragma once

#include "../renderer/HkRenderContext.hpp"
#include "../renderer/HkTransformContext.hpp"

namespace hkui
{
class HkNodeData
{
public:
    HkNodeData() {}

    HkRenderContext renderContext;
    HkTransformContext transformContext;
};
} // hkui