#pragma once

#include <string>
#include <memory>

#include "../base/IHkRootNode.hpp"
#include "../base/HkNodeBase.hpp"
#include "../base/HkNodeData.hpp"
#include "../base/HkTreeStructure.hpp"
#include "WindowFrameHelpers/HkWFContainer.hpp"

namespace hkui
{
/* This shall serve as the root of the scene manager, it has no parent and can only host children.*/
class HkWindowFrame : public HkNodeBase, public IHkRootNode
{
public:
    HkWindowFrame(const std::string& windowName);

    void rootUpdateMySelf() override; // IHkRootNode
    // void updateMySelf() override; // IHkNodeBase

    virtual void onGeneralMouseMove() override; // HkNodeBase
    virtual void onGeneralMouseClick() override; // HkNodeBase
    virtual void onGeneralUpdate() override; // HkNodeBase


    /* add whatever functionality.. */
    void updateChildren();
    void pushChildren(const std::vector<HkNodeBasePtr>& newChildren);
    void printTree();
    void setColor(const glm::vec3& color);
    void setPos(const glm::vec2& pos);
    void setSize(const glm::vec2& size);

private:
    HkWFContainer wfCont_;
};
using HkWindowFramePtr = std::shared_ptr<HkWindowFrame>;
using HkWindowFrameCPtr = const std::shared_ptr<HkWindowFrame>;
} // hkui

