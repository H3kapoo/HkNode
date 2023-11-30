#pragma once

#include "../contexts/HkRenderContext.hpp"
#include "../contexts/HkTransformContext.hpp"
#include "../contexts/HkConstraintContext.hpp"
#include "../contexts/HkStyleContext.hpp"
#include "../contexts/HkEventsContext.hpp"

namespace hkui
{
class HkNodeData
{
public:
    HkRenderContext renderContext;
    HkTransformContext transformContext;
    HkTransformContext borderTransformContext;
    HkConstraintContext constraintContext;
    HkStyleContext styleContext;
    HkEventsContext eventsContext;
};
} // hkui