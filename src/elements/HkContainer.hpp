#pragma once

#include <functional>

#include "../base/HkNodeBase.hpp"
#include "helpers/HkScrollBar.hpp"
#include "../utils/HkPinchHelper.hpp"

namespace hkui
{
class HkContainer : public HkNodeBase
{
public:
    HkContainer(const std::string& containerName);
    void pushChildren(const std::vector<HkNodeBasePtr>& newChildren);

private:
    void onFirstHeartbeat() override;
    void onScroll() override;
    void onResolveFocusPass() override;
    void onGeneralMouseScroll() override;
    void onClick() override;
    void onRelease() override;
    void onGeneralMouseClickOrRelease() override;
    void onGeneralMouseMove() override;
    void onDrag() override;
    void resolveChildrenConstraints(HkTreeStruct& children,
        const HkScrollbarsSize&) override;
    void postRenderAdditionalDetails() override;

    void constrainScrollbarsIfNeeded();
    void resolveScrollBarChildrenIfNeeded();
    //TODO: maybe push those ^^ in constraint mgmt

private:
    HkScrollBar hScrollBar_;
    HkScrollBar vScrollBar_;
    uint32_t scrollbBarsCount_;
    HkNodeData dummyXYIntersectorData_;

    HkPinchHelper pinchHelper_;
};
using HkContainerPtr = std::shared_ptr<HkContainer>;
using HkContainerCPtr = const std::shared_ptr<HkContainer>;
} // hkui