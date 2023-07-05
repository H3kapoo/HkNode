#pragma once

#include <string>

#include "../base/HkNode.hpp"

namespace hkui
{
    class HkContainer : public HkNode
    {
    public:
        HkContainer(const std::string& name) : HkNode(name, "Container") {}

        //HkNode
        void updateMySelf() override
        {
            // std::cout << getName() << " needs an update\n";
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


    class HkButton : public HkNode
    {
    public:
        HkButton(const std::string& name) : HkNode(name, "Button") {}

        //HkNode
        void updateMySelf() override
        {
            std::cout << getName() << " " << std::boolalpha << HkSceneManagement::sceneData.isMouseClicked << "\n";
        }
    };

    using HkButtonPtr = std::shared_ptr<HkButton>;
    using HkButtonCPtr = const std::shared_ptr<HkButton>;

    using HkContainerPtr = std::shared_ptr<HkContainer>;
    using HkContainerCPtr = const std::shared_ptr<HkContainer>;
} // hkui