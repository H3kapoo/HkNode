#pragma once

#include "../renderer/HkRenderContext.hpp"
#include "../renderer/HkTransformContext.hpp"
#include "../renderer/HkConstraintContext.hpp"

namespace hkui
{
class HkNodeData
{
public:
    HkRenderContext renderContext;
    HkTransformContext transformContext;
    HkConstraintContext constraintContext;
};
} // hkui