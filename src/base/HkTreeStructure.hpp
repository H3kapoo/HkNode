#pragma once

#include <vector>
#include <algorithm>
#include <memory>
#include <set>
#include <string>
#include <queue>
#include <iostream>

namespace hkui
{

template <typename T>
class HkTreeStructure
{
public:
    // think about reparent later
    // static void reParent(const std::vector<std::shared_ptr<HkTreeStructure>>& nodes, const std::shared_ptr<HkTreeStructure> newParent);

    HkTreeStructure(T* payload, const std::string& name, const std::string& type)
        : name_{ name }, type_{ type }, id_{ genId() }, payload_{ payload }, parent_{ nullptr }
    {}

    void removeChildren(const std::set<uint32_t>& rmChildrenIds)
    {
        std::vector<HkTreeStructure*> interestingTees;
        interestingTees.reserve(rmChildrenIds.size());
        const auto it = std::remove_if(children_.begin(), children_.end(),
            [&rmChildrenIds, &interestingTees](HkTreeStructure* tee)
            {
                const bool shouldRemove = rmChildrenIds.count(tee->id_);
                if (shouldRemove) interestingTees.push_back(tee);
                return shouldRemove;
            });

        for (const auto& tee : interestingTees)
        {
            tee->setParent(nullptr);
        }
        children_.erase(it, children_.end());
    }

    void pushChild(HkTreeStructure<T>* child)
    {
        if (child)
        {
            if (child->isParented_ || child->getId() == id_) return;
            child->setParent(this);
            children_.push_back(child);
        }
    }

    void printChildren() const;
    void printBfs() const;

    void printTree() const { printTreeAux(0); }

    std::shared_ptr<HkTreeStructure> hasChild(uint32_t childId) const;

    // /*Overloads*/
    template<class U>
    friend std::ostream& operator<<(std::ostream& os, const HkTreeStructure<U>* node)
    {
        os << "(" << node->type_ << ": " << node->name_ << ", Id = " << node->id_ << ")";
        return os;
    }

    // GET(std::string, Name, name);
    // GET(uint32_t, Id, id);
    // GET(uint32_t, Level, level);
    uint32_t getId() { return id_; }
    HkTreeStructure* getParent() { return parent_; }
    std::vector<HkTreeStructure*> getChildren() { return children_; }
    T* getPayload() { return payload_; }

private:
    void printTreeAux(int level) const
    {
        for (int x = 0; x < level - 1; x++)
            std::cout << "    ";

        if (level == 0)
            std::cout << "| " << this << '\n';
        else
            std::cout << "----| " << this << '\n';

        for (const auto& node : children_)
        {
            node->printTreeAux(level + 1);
        }
    }

    static uint32_t genId()
    {
        static uint32_t currentId = 1; // Start giving IDs from 1
        return currentId++;
    }

    void setParent(HkTreeStructure<T>* newParent)
    {
        parent_ = newParent;
        level_ = newParent != nullptr ? newParent->level_ + 1 : 0;
        isParented_ = newParent != nullptr ? true : false;
    }

    std::string name_{ "UNSET_NODE" };
    std::string type_{ "UNSET_TYPE" };
    uint32_t id_{ 0 };
    uint32_t level_{ 0 };
    bool isParented_{ false };

    T* payload_;
    HkTreeStructure* parent_;
    std::vector<HkTreeStructure<T>*> children_;
};
} // hkui