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
    void onGeneralUpdate() override;
    void onGeneralMouseMove() override;

    void resolveConstraints(std::vector<HkTreeStructure<HkNodeBase>*>& children);

    void pushChildren(const std::vector<HkNodeBasePtr>& newChildren);
    void scrollbars(bool x, bool y);

    void setColor(const glm::vec3& color);
    void setPos(const glm::vec2& pos);
    void setSize(const glm::vec2& size);

private:
    //TODO: Maybe allocate them on demand? but then hiding and re-showing would need
    //      realloc again..
    HkScrollBar hScrollBar_;
    HkScrollBar vScrollBar_;
    uint32_t sbCount_;
};
using HkContainerPtr = std::shared_ptr<HkContainer>;
using HkContainerCPtr = const std::shared_ptr<HkContainer>;
} // hkui