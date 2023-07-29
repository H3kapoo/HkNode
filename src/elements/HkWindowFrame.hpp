#pragma once

#include <string>
#include <memory>

#include "IHkRootNode.hpp"
#include "HkNodeBase.hpp"
#include "../base/HkNodeData.hpp"
#include "../base/HkTreeStructure.hpp"

namespace hkui
{
/* This shall serve as the root of the scene manager, it has no parent and can only host children.*/
class HkWindowFrame : protected HkNodeBase, public IHkRootNode
{
public:
    HkWindowFrame(const std::string& windowName);

    void rootUpdateMySelf() override; // IHkRootNode
    void updateMySelf() override; // IHkNodeBase

    /* add whatever functionality.. */
    void updateChildren();
    void pushChildren(const std::vector<HkNodeBasePtr>& newChildren);
    void printTree();
    void setColor(const glm::vec3& color);
    void setPos(const glm::vec2& pos);
    void setSize(const glm::vec2& size);


private:
    // todo
};
using HkWindowFramePtr = std::shared_ptr<HkWindowFrame>;
using HkWindowFrameCPtr = const std::shared_ptr<HkWindowFrame>;
} // hkui

