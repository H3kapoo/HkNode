#pragma once

#include <string>
#include <memory>

#include "IHkRootNode.hpp"
#include "../management/HkSceneManagement.hpp"
#include "../base/HkNode.hpp"

#include "WindowFrameHelpers/HkWFContainer.hpp"
#include "HkButton.hpp"

namespace hkui
{
/* This shall serve as the root of the scene manager, it has no parent and can only host children.*/
// TODO: Make sure you cannot pushChildren of type HkWindowFrame
class HkWindowFrame : public HkNode, public IHkRootNode
{
public:
    HkWindowFrame(const std::string& windowName);

    // IHkRootNode
    void rootUpdateMySelf() override;

    // HkNode
    void updateMySelf() override;

    void updateChildren();
    void pushChildren(const std::vector<HkNodePtr>& newChildren);

    void printTree();
    void setColor(const glm::vec3& color);
    void setPos(const glm::vec2& pos);
    void setSize(const glm::vec2& size);

private:
    //gatekeep
    void gateKeep() { std::cout << "sugi\n"; }

private:
    void renderAdditionalSelfElements();

    HkSceneData& sceneDataRef; /* This is safe as singleton will outlive THIS class anyway*/

    /* Elements that compose the "tab view" of the window frame*/
    bool elementsInitialized;
    HkWFContainerPtr wfContainer;
    HkButtonPtr exitBtnNode;
};

using HkWindowFramePtr = std::shared_ptr<HkWindowFrame>;
using HkWindowFrameCPtr = const std::shared_ptr<HkWindowFrame>;

} // hkui

