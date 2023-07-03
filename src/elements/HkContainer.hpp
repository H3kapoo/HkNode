#pragma once

#include <string>

#include "../base/HkNode.hpp"

namespace hkui
{
    // addons
    struct IHkElementHolder
    {
        virtual void pushChildren(const std::vector<std::shared_ptr<HkNode>>&) = 0;
        virtual void updateChildren() = 0;
    };

    struct IHkElementPrintable
    {
        virtual void printTree() = 0;
    };

    class IHkElementUtility
    {
    public:
        virtual void identify(std::ostream&) = 0;

        friend std::ostream& operator<<(std::ostream& os, const std::shared_ptr<IHkElementUtility>& element)
        {
            element->identify(os);
            return os;
        }
    };
    //

    class HkContainer : public HkNode
        , public IHkElementUtility
        , public IHkElementPrintable
        , public IHkElementHolder
    {
    public:
        HkContainer(const std::string& name) : HkNode(name, "Container") {}

        //HkNode
        void updateMySelf() override
        {
            std::cout << getName() << " needs an update\n";
            updateChildren();
        }


        // IHkElementHolder
        void updateChildren() override
        {
            for (const auto& child : getChildren())
                child->updateMySelf();
        }

        void pushChildren(const std::vector<std::shared_ptr<HkNode>>& newChildren) override
        {
            HkNode::pushChildren(newChildren);
        }

        // IHkElementPrintable
        void printTree() override { HkNode::printTree(); }
    private:
        // HkElement
        void identify(std::ostream& os) override { os << this; }
    };

    class HkButton : public HkNode
        , public IHkElementUtility
    {
    public:
        HkButton(const std::string& name) : HkNode(name, "Button") {}

        //HkNode
        void updateMySelf() override
        {
            std::cout << getName() << " button needs an update\n";
        }

    private:
        // HkElement
        void identify(std::ostream& os) override { os << this; }
    };

    using HkButtonPtr = std::shared_ptr<HkButton>;
    using HkButtonCPtr = const std::shared_ptr<HkButton>;

    using HkContainerPtr = std::shared_ptr<HkContainer>;
    using HkContainerCPtr = const std::shared_ptr<HkContainer>;
} // hkui