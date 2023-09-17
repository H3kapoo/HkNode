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

private:
    /* Invokers */
    void invokeMouseEvent(int32_t x, int32_t y, HkMouseAction action, HkMouseButton btn);

    /* Storage (not sure if they should be const& also, maybe they will get lost) */
    std::function<void()> basicClickCb;
    std::function<void()> basicReleaseCb;
    std::function<void(int32_t x, int32_t y, HkMouseAction action, HkMouseButton btn)> mouseCallback;

    //TODO: Maybe to minimize memory, we could create the callbacks on demand
};
} // hkui