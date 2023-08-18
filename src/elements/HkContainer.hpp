#pragma once

#include "../management/HkSceneManagement.hpp"
#include "../base/HkNodeBase.hpp"
#include "helpers/HkScrollBar.hpp"

namespace hkui
{
class HkContainer : public HkNodeBase
{
public:
    HkContainer(const std::string& containerName);

    /* HkNodeBase */
    void resolveChildrenConstraints(std::vector<HkTreeStructure<HkNodeBase>*>& children) override;
    void postRenderAdditionalDetails() override;

    void onDrag() override;
    void onGeneralMouseMove() override;
    void onGeneralMouseClick() override;

    void constrainScrollbarsIfNeeded();
    void handleContainerOverflowIfNeeded();
    //TODO: maybe push those ^^ in constraint mgmt

    void pushChildren(const std::vector<HkNodeBasePtr>& newChildren);

    void setColor(const glm::vec3& color);
    void setPos(const glm::vec2& pos);
    void setSize(const glm::vec2& size);

private:
    //TODO: Maybe allocate them on demand? but then hiding and re-showing would need
    //      realloc again..
    HkScrollBar hScrollBar_;
    HkScrollBar vScrollBar_;
    uint32_t scrollbBarsCount_;

    HkNodeData dummyXYIntersectorData_;
};
using HkContainerPtr = std::shared_ptr<HkContainer>;
using HkContainerCPtr = const std::shared_ptr<HkContainer>;
} // hkui