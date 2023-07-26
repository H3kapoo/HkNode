#pragma once

#include <string>
#include <memory>

#include "IHkRootNode.hpp"
#include "HkNodeBase.hpp"
#include "../management/HkSceneManagement.hpp"
#include "../base/HkNodeData.hpp"

#include "../base/HkTreeStructure.hpp"

namespace hkui
{
/* This shall serve as the root of the scene manager, it has no parent and can only host children.*/
class HkWindowFrame : public HkNodeBase, public IHkRootNode
{
public:
    HkWindowFrame(const std::string& windowName);

    void updateChildren();
    void pushChildren(const std::vector<HkNodeBasePtr>& newChildren);
    void printTree();
    void setColor(const glm::vec3& color);
    void setPos(const glm::vec2& pos);
    void setSize(const glm::vec2& size);

    void rootUpdateMySelf() override; // IHkRootNode
    void updateMySelf() override; // IHkNodeBase

private:
    void renderAdditionalSelfElements();

    HkSceneData& sceneDataRef; /* This is safe as singleton will outlive THIS class anyway*/

    /* Elements that compose the "tab view" of the window frame*/
    // HkWFContainerPtr wfContainer;
    // HkButtonPtr exitBtnNode;
};
using HkWindowFramePtr = std::shared_ptr<HkWindowFrame>;
using HkWindowFrameCPtr = const std::shared_ptr<HkWindowFrame>;
} // hkui

