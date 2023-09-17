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
    const std::function<void(int32_t, int32_t, HkMouseAction, HkMouseButton)>& callback)
{
    mouseCallback = callback;
    return *this;
}

HkEventsContext& HkEventsContext::setOnMouseMoveListener(const std::function<void()>& callback)
{
    basicHoverCallback = callback;
    return *this;
}

HkEventsContext& HkEventsContext::setOnMouseMoveListener(const std::function<void(int32_t, int32_t)>& callback)
{
    hoverCallback = callback;
    return *this;
}

HkEventsContext& HkEventsContext::setOnMouseExitListener(const std::function<void()>& callback)
{
    basicMouseExitCallback = callback;
}

HkEventsContext& HkEventsContext::setOnMouseEnterListener(const std::function<void()>& callback)
{
    basicMouseEnterCallback = callback;
}

void HkEventsContext::invokeMouseEvent(int32_t x, int32_t y, HkMouseAction action, HkMouseButton btn)
{
    /* Explicitly only handle movement related events*/
    if (action == HkMouseAction::Move)
    {
        if (basicHoverCallback) { basicHoverCallback(); }
        if (hoverCallback) { hoverCallback(x, y); }
        return;
    }

    /* Explicit handling of enter/exit and premature return*/
    if (action == HkMouseAction::Entered)
    {
        if (basicMouseEnterCallback) { basicMouseEnterCallback(); }
        return;
    }

    if (action == HkMouseAction::Exited)
    {
        if (basicMouseExitCallback) { basicMouseExitCallback(); }
        return;
    }

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