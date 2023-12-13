#include <iostream>

#include "HkEventsContext.hpp"

namespace hkui
{
HkEventsContext& HkEventsContext::setOnClickListener(const std::function<void()>& callback)
{
    basicClickCb_ = callback;
    return *this;
}

HkEventsContext& HkEventsContext::setOnClickListener(
    const std::function<void(int32_t, int32_t, HkMouseAction, HkMouseButton)>& callback)
{
    mouseCallback_ = callback;
    return *this;
}

HkEventsContext& HkEventsContext::setOnReleaseListener(const std::function<void()>& callback)
{
    basicReleaseCb_ = callback;
    return *this;
}

HkEventsContext& HkEventsContext::setOnMoveListener(const std::function<void()>& callback)
{
    basicHoverCallback_ = callback;
    return *this;
}

HkEventsContext& HkEventsContext::setOnMoveListener(const std::function<void(int32_t, int32_t)>& callback)
{
    hoverCallback_ = callback;
    return *this;
}

HkEventsContext& HkEventsContext::setOnExitListener(const std::function<void()>& callback)
{
    basicMouseExitCallback_ = callback;
    return *this;
}

HkEventsContext& HkEventsContext::setOnEnterListener(const std::function<void()>& callback)
{
    basicMouseEnterCallback_ = callback;
    return *this;
}

HkEventsContext& HkEventsContext::setOnScrollListener(const std::function<void()>& callback)
{
    basicMouseScrollCallback_ = callback;
    return *this;
}

HkEventsContext& HkEventsContext::setOnScrollListener(const std::function<void(int32_t value)>& callback)
{
    mouseScrollCallback_ = callback;
    return *this;
}

void HkEventsContext::invokeMouseEvent(int32_t x, int32_t y, HkMouseAction action, HkMouseButton btn)
{
    /* Explicitly only handle movement related events*/
    if (action == HkMouseAction::Move)
    {
        if (basicHoverCallback_) { basicHoverCallback_(); }
        if (hoverCallback_) { hoverCallback_(x, y); }
        return;
    }

    /* Explicitly only handle scrolling related events*/
    if (action == HkMouseAction::Scroll)
    {
        if (basicMouseScrollCallback_) { basicMouseScrollCallback_(); }
        if (mouseScrollCallback_) { mouseScrollCallback_(y); } /* Only Y scroll*/
        return;
    }

    /* Explicit handling of enter/exit and premature return*/
    if (action == HkMouseAction::Entered && !mouseIsInside_)
    {
        mouseIsInside_ = true;
        if (basicMouseEnterCallback_) { basicMouseEnterCallback_(); }
        return;
    }

    if (action == HkMouseAction::Exited && mouseIsInside_)
    {
        mouseIsInside_ = false;
        if (basicMouseExitCallback_) { basicMouseExitCallback_(); }
        return;
    }

    if (btn == HkMouseButton::Left)
    {
        if (basicClickCb_ && action == HkMouseAction::Click)
        {
            basicClickCb_();
        }
        else if (basicReleaseCb_ && action == HkMouseAction::Release)
        {
            basicReleaseCb_();
        }
    }
    /* Call complex in the end always if it exists*/
    if (mouseCallback_) { mouseCallback_(x, y, action, btn); }
}
} //hkui