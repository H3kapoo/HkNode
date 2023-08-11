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
    void resolveConstraints(std::vector<HkTreeStructure<HkNodeBase>*>& children);
    void postChildrenRendered();

    void onGeneralMouseMove();
    void onGeneralMouseClick();

    void constrainScrollbarsIfNeeded();
    void handleContainerOverflowIfNeeded();
    void pushChildren(const std::vector<HkNodeBasePtr>& newChildren);

    void setColor(const glm::vec3& color);
    void setPos(const glm::vec2& pos);
    void setSize(const glm::vec2& size);

private:
    //TODO: Maybe allocate them on demand? but then hiding and re-showing would need
    //      realloc again..
    HkScrollBar hScrollBar_;
    HkScrollBar vScrollBar_;
    uint32_t sbCount_;

    HkNodeData dummyXYIntersectorData_;
    bool mouseClickPositionSet_{ false };
    glm::ivec2 clickPosition_{ 0,0 };
    bool isIntersectorNeeded_;
};
using HkContainerPtr = std::shared_ptr<HkContainer>;
using HkContainerCPtr = const std::shared_ptr<HkContainer>;
} // hkui