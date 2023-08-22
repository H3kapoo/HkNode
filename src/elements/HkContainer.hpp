#pragma once

#include <functional>

#include "../management/HkSceneManagement.hpp"
#include "../base/HkNodeBase.hpp"
#include "helpers/HkScrollBar.hpp"

namespace hkui
{
class HkContainer : public HkNodeBase
{
public:
    HkContainer(const std::string& containerName);

    void onClick() override;
    void onDrag() override;
    void resolveChildrenConstraints(std::vector<HkTreeStructure<HkNodeBase>*>& children) override;
    void postRenderAdditionalDetails() override;

    void constrainScrollbarsIfNeeded();
    void resolveScrollBarChildrenIfNeeded();
    //TODO: maybe push those ^^ in constraint mgmt
    // Or SB dedicated constraint context?
    void pushChildren(const std::vector<HkNodeBasePtr>& newChildren);

    void setOnClickListener(std::function<void()> callback);

    /* Setters */
    void setColor(const glm::vec3& color);
    void setPos(const glm::vec2& pos);
    void setSize(const glm::vec2& size);

private:
    HkScrollBar hScrollBar_;
    HkScrollBar vScrollBar_;
    uint32_t scrollbBarsCount_;
    HkNodeData dummyXYIntersectorData_;

    std::function<void()> onClickCallback_{ nullptr };
};
using HkContainerPtr = std::shared_ptr<HkContainer>;
using HkContainerCPtr = const std::shared_ptr<HkContainer>;
} // hkui