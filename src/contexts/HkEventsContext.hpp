#pragma once

#include <functional>

#include "../management/HkEventStructs.hpp"

namespace hkui
{
class HkEventsContext
{
    friend class HkNodeBase;

public:

    /* Setters */

    /* Basic, Left mouse click based */
    HkEventsContext& setOnClickListener(const std::function<void()>& callback);
    /* Complex mouse response */
    HkEventsContext& setOnClickListener(const std::function<void(int32_t x, int32_t y,
        HkMouseAction action, HkMouseButton btn)>& callback);
    /* Basic, Left mouse release based */
    HkEventsContext& setOnReleaseListener(const std::function<void()>& callback);

    /* Basic hover callback */
    HkEventsContext& setOnMoveListener(const std::function<void()>& callback);
    /* Complex hover callback */
    HkEventsContext& setOnMoveListener(const std::function<void(int32_t x, int32_t y)>& callback);

    /* Basic mouse enter/exit*/ //TODO: To be implemented when inside window pane grabbing comes
    HkEventsContext& setOnExitListener(const std::function<void()>& callback);
    HkEventsContext& setOnEnterListener(const std::function<void()>& callback);

    /* Basic mouse scroll*/
    HkEventsContext& setOnScrollListener(const std::function<void()>& callback);
    HkEventsContext& setOnScrollListener(const std::function<void(int32_t value)>& callback);

private:
    /* Invokers */
    void invokeMouseEvent(int32_t x, int32_t y, HkMouseAction action, HkMouseButton btn);

    /* Storage (not sure if they should be const& also, maybe they will get lost) */
    std::function<void()> basicClickCb;
    std::function<void()> basicReleaseCb;
    std::function<void(int32_t x, int32_t y, HkMouseAction action, HkMouseButton btn)> mouseCallback;

    std::function<void()> basicHoverCallback;
    std::function<void(int32_t x, int32_t y)> hoverCallback;

    std::function<void()> basicMouseExitCallback;
    std::function<void()> basicMouseEnterCallback;

    std::function<void()> basicMouseScrollCallback;
    std::function<void(int32_t x)> mouseScrollCallback;

    //TODO: Maybe to minimize memory, we could create the callbacks on demand
};
} // hkui