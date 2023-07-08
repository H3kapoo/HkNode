#pragma once

#include <string>

#include "../management/HkSceneManagement.hpp"
#include "../renderer/HkRenderer.hpp"
#include "../base/HkNode.hpp"

namespace hkui
{

//TODO: maybe rename HkNode => HkTreeNode
class HkContainer : public HkNode, public HkRenderableNode
{
public:
    HkContainer(const std::string& name)
        : HkNode(name, "Container")
        , HkRenderableNode("assets/shaders/v1.glsl", "assets/shaders/f1.glsl")
    {
        HkRenderableNode::render();
    }

    //HkNode
    void updateMySelf() override
    {
        /*main HkEvents handler*/
        switch (HkSceneManagement::sceneData.currentEvent)
        {
        case HkEvent::None:
        case HkEvent::GeneralUpdate: break;
        case HkEvent::WindowResize:
        case HkEvent::MouseMove:
            // break;
        case HkEvent::MouseClick:
            if (!HkSceneManagement::sceneData.isMouseClicked)
            {
                // std::cout << "mouse released at: " <<
                    // HkSceneManagement::sceneData.mousePosX << " " << HkSceneManagement::sceneData.mousePosY << '\n';
                HkRenderableNode::setPos({ HkSceneManagement::sceneData.mousePosX, HkSceneManagement::sceneData.mousePosY });
            }
            break;
        case HkEvent::MouseEnterExit:
        case HkEvent::MouseScroll:
        case HkEvent::DropPath:
            break;
        }
        HkRenderableNode::render();
        updateChildren();
    }

    void updateChildren()
    {
        for (const auto& child : getChildren())
            child->updateMySelf();
    }

    void pushChildren(const std::vector<std::shared_ptr<HkNode>>& newChildren)
    {
        HkNode::pushChildren(newChildren);
    }

    void printTree() { HkNode::printTree(); }
};


class HkButton : public HkNode, public HkRenderableNode
{
public:
    HkButton(const std::string& name)
        : HkNode(name, "Button")
        , HkRenderableNode("assets/shaders/v1.glsl", "assets/shaders/f1.glsl")
    {
        HkNodeRenderer::renderNode(this);
    }

    //HkNode
    void updateMySelf() override
    {
        switch (HkSceneManagement::sceneData.currentEvent)
        {
        case HkEvent::None:
            std::cout << this << " none evt\n";
            break;
        case HkEvent::GeneralUpdate:
            std::cout << this << " gen update\n";
            break;
        case HkEvent::WindowResize:
            std::cout << this << " win resize\n";
            break;
        case HkEvent::MouseMove:
            std::cout << this << " mouse move\n";
            break;
        case HkEvent::MouseClick:
            if (HkSceneManagement::sceneData.isMouseClicked)
                std::cout << this << " mouse clicked\n";
            break;
        case HkEvent::MouseEnterExit:
            std::cout << this << " mouse enter/exit\n";
            break;
        case HkEvent::MouseScroll:
            std::cout << this << " mouse scroll\n";
            break;
        case HkEvent::DropPath:
            std::cout << this << " drop path\n";
            break;
        }
        HkNodeRenderer::renderNode(this);
    }
};

using HkButtonPtr = std::shared_ptr<HkButton>;
using HkButtonCPtr = const std::shared_ptr<HkButton>;

using HkContainerPtr = std::shared_ptr<HkContainer>;
using HkContainerCPtr = const std::shared_ptr<HkContainer>;
} // hkui