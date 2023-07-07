#pragma once

#include <string>

#include "../management/HkSceneManagement.hpp"
#include "../renderer/HkRenderer.hpp"
#include "../base/HkNode.hpp"

namespace hkui
{
    class HkContainer : public HkNode, public HkRenderableNode
    {
    public:
        HkContainer(const std::string& name) : HkNode(name, "Container") { HkNodeRenderer::renderNode(this); }

        //HkNode
        void updateMySelf() override
        {
            // std::cout << getName() << " needs an update\n";
            HkNodeRenderer::renderNode(this);
            switch (HkSceneManagement::sceneData.currentEvent)
            {
            case Event::None:
            case Event::GeneralUpdate:
            case Event::WindowResize:
            case Event::MouseMove:
            case Event::MouseClick:
            case Event::MouseEnterExit:
            case Event::MouseScroll:
            case Event::DropPath:
                break;
            }
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
        HkButton(const std::string& name) : HkNode(name, "Button") { HkNodeRenderer::renderNode(this); }

        //HkNode
        void updateMySelf() override
        {
            HkNodeRenderer::renderNode(this);
            switch (HkSceneManagement::sceneData.currentEvent)
            {
            case Event::None:
                std::cout << this << " none evt\n";
                break;
            case Event::GeneralUpdate:
                std::cout << this << " gen update\n";
                break;
            case Event::WindowResize:
                std::cout << this << " win resize\n";
                break;
            case Event::MouseMove:
                std::cout << this << " mouse move\n";
                break;
            case Event::MouseClick:
                if (HkSceneManagement::sceneData.isMouseClicked)
                    std::cout << this << " mouse clicked\n";
                break;
            case Event::MouseEnterExit:
                std::cout << this << " mouse enter/exit\n";
                break;
            case Event::MouseScroll:
                std::cout << this << " mouse scroll\n";
                break;
            case Event::DropPath:
                std::cout << this << " drop path\n";
                break;
            }
        }
    };

    using HkButtonPtr = std::shared_ptr<HkButton>;
    using HkButtonCPtr = const std::shared_ptr<HkButton>;

    using HkContainerPtr = std::shared_ptr<HkContainer>;
    using HkContainerCPtr = const std::shared_ptr<HkContainer>;
} // hkui