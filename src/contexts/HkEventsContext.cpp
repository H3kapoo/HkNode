#include <iostream>

#include "HkEventsContext.hpp"

namespace hkui
{
HkEventsContext& HkEventsContext::setOnClickListener(const std::function<void()>& callback)
{
    basicClickCb = callback;
    return *this;
}

HkEventsContext& HkEventsContext::setOnReleaseListener(const std::function<void()>& callback)
{
    basicReleaseCb = callback;
    return *this;
}

HkEventsContext& HkEventsContext::setOnMouseListener(
    const std::function<void(int32_t x, int32_t y, HkMouseAction action, HkMouseButton btn)>& callback)
{
    mouseCallback = callback;
    return *this;
}

void HkEventsContext::invokeMouseEvent(int32_t x, int32_t y, HkMouseAction action, HkMouseButton btn)
{
    if (btn == HkMouseButton::Left)
    {
        if (basicClickCb && action == HkMouseAction::Click)
        {
            basicClickCb();
        }
        else if (basicReleaseCb && action == HkMouseAction::Release)
        {
            basicReleaseCb();
        }
    }

    /* Call complex in the end always if it exists*/
    if (mouseCallback) { mouseCallback(x, y, action, btn); }
}
} //hkui