#pragma once

#include <vector>
#include <algorithm>
#include <memory>
#include <set>
#include <string>
#include <queue>
#include <iostream>

#include "HkNodeData.hpp"

#define GET(typ, n, x) typ get##n() { return x;}

namespace hkui
{
class HkNode : public HkNodeData, public std::enable_shared_from_this<HkNode>
{
public:
    HkNode(const std::string& name, const std::string& type);
    static void reParent(const std::vector<std::shared_ptr<HkNode>>& nodes, const std::shared_ptr<HkNode> newParent);
    virtual ~HkNode() = default;
    virtual void updateMySelf() = 0;

    void removeChildren(const std::set<uint32_t>& rmChildrenIds);

    // virtual void gateKeepPure() = 0;

    /*Currently we can push only shared ptrs of HkNode, so pushing a list of HkNode& would not work
      if we didnt want to create heap allocated nodes
      TODO: maybe handle also pushing stack allocated nodes */
    void pushChildren(const std::vector<std::shared_ptr<HkNode>>& newChildren);
    void printChildren() const;
    void printBfs() const;
    void printTree() const;
    std::shared_ptr<HkNode> hasChild(uint32_t childId) const;


    GET(std::string, Name, name);
    GET(uint32_t, Id, id);
    GET(uint32_t, Level, level);
    GET(std::weak_ptr<HkNode>, Parent, parent);
    GET(std::vector<std::shared_ptr<HkNode>>, Children, children);

    friend std::ostream& operator<<(std::ostream& os, const std::shared_ptr<HkNode>& node);
    friend std::ostream& operator<<(std::ostream& os, const HkNode& node);
    friend std::ostream& operator<<(std::ostream& os, const HkNode* node);

private:
    void printTreeAux(int level) const;
    static uint32_t genId();
    void setParent(const std::shared_ptr<HkNode>& newParent);

    std::string name{ "UNSET_NODE" };
    std::string type{ "UNSET_TYPE" };
    uint32_t id{ 0 };
    uint32_t level{ 0 };
    bool isParented{ false };
    std::weak_ptr<HkNode> parent;
    std::vector<std::shared_ptr<HkNode>> children;
};

using HkNodePtr = std::shared_ptr<HkNode>;
using HkNodeCPtr = const std::shared_ptr<HkNode>;
} // hkui