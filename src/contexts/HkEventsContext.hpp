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
    /* Basic, Left mouse release based */
    HkEventsContext& setOnReleaseListener(const std::function<void()>& callback);
    /* Complex mouse response */
    HkEventsContext& setOnMouseListener(const std::function<void(int32_t x, int32_t y,
        HkMouseAction action, HkMouseButton btn)>& callback);

    /* Basic hover callback */
    HkEventsContext& setOnMouseMoveListener(const std::function<void()>& callback);
    /* Complex hover callback */
    HkEventsContext& setOnMouseMoveListener(const std::function<void(int32_t x, int32_t y)>& callback);

    /* Basic mouse enter/exit*/
    HkEventsContext& setOnMouseExitListener(const std::function<void()>& callback);
    HkEventsContext& setOnMouseEnterListener(const std::function<void()>& callback);

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

    //TODO: Maybe to minimize memory, we could create the callbacks on demand
};
} // hkui