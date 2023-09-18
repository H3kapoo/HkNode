#pragma once

#include <functional>

#include "../management/HkSceneManagement.hpp" // TODO: Needed?
#include "../base/HkNodeBase.hpp"
#include "helpers/HkScrollBar.hpp"

namespace hkui
{
class HkContainer : public HkNodeBase
{
public:
    HkContainer(const std::string& containerName);
    void pushChildren(const std::vector<HkNodeBasePtr>& newChildren);

    /* Setters */
    void setOnClickListener(std::function<void()> callback);

    void setSize(const glm::vec2& size);

private:
    void onScroll() override;
    void onGeneralMouseScroll() override;
    void onClick() override;
    void onGeneralMouseClick() override;
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
};
using HkContainerPtr = std::shared_ptr<HkContainer>;
using HkContainerCPtr = const std::shared_ptr<HkContainer>;
} // hkui