#pragma once

#include "../contexts/HkRenderContext.hpp"
#include "../contexts/HkTransformContext.hpp"
#include "../contexts/HkConstraintContext.hpp"

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