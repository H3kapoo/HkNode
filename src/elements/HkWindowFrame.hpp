#pragma once

#include <string>
#include <memory>

#include "../base/IHkRootNode.hpp"
#include "../base/HkNodeBase.hpp"
#include "../base/HkNodeData.hpp"
#include "../base/HkTreeStructure.hpp"
#include "HkContainer.hpp"
#include "helpers/HkScrollBar.hpp"

namespace hkui
{
class HkWindowFrame : public HkNodeBase, public IHkRootNode
{
public:
    HkWindowFrame(const std::string& windowName);

    /* IHRootNode */
    void rootUpdateMySelf() override;

    /* HkNodeBase */
    void onDrag() override;
    void onWindowResize() override;
    void resolveChildrenConstraints(std::vector<HkTreeStructure<HkNodeBase>*>& children) override;

    void pushChildren(const std::vector<HkNodeBasePtr>& newChildren);
    void printTree();

    /* Setters */
    void setColor(const glm::vec3& color);
    void setPos(const glm::vec2& pos);
    void setSize(const glm::vec2& size);
    void setConstraintPolicy(const HkConstraintPolicy policy);
private:
    HkContainer wfCont_;
};
using HkWindowFramePtr = std::shared_ptr<HkWindowFrame>;
using HkWindowFrameCPtr = const std::shared_ptr<HkWindowFrame>;
} // hkui

