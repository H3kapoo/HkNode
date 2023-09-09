#pragma once

#include "../contexts/HkRenderContext.hpp"
#include "../contexts/HkTransformContext.hpp"
#include "../contexts/HkConstraintContext.hpp"
#include "../contexts/HkStyleContext.hpp"

namespace hkui
{
class HkNodeData
{
public:
    HkRenderContext renderContext;
    HkTransformContext transformContext;
    HkConstraintContext constraintContext;
    HkStyleContext styleContext;
};
} // hkui