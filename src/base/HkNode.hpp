#pragma once

#include <vector>
#include <algorithm>
#include <memory>
#include <queue>
#include <set>
#include <utility>

#define GET(typ, n, x) typ get##n() { return x;}

class HkNode : public std::enable_shared_from_this<HkNode>
{
public:
    HkNode(const std::string& name);
    void removeChildren(const std::set<uint32_t>& rmChildrenIds);
    void pushChildren(const std::vector<std::shared_ptr<HkNode>>& newChildren);
    void printChildren() const;
    void printBfs(const std::shared_ptr<HkNode> srcNode) const;
    static void reParent(const std::vector<std::shared_ptr<HkNode>>& nodes, const std::shared_ptr<HkNode> newParent);
    std::shared_ptr<HkNode> hasChild(uint32_t childId) const;

    GET(std::string, Name, name);
    GET(uint32_t, Id, id);
    GET(uint32_t, Level, level);
    GET(std::weak_ptr<HkNode>, Parent, parent);
    GET(std::vector<std::shared_ptr<HkNode>>, Children, children);

private:
    static uint32_t genId();
    void setParent(const std::shared_ptr<HkNode>& newParent);

    std::string name{ "UNSET_NODE" };
    uint32_t id{ 0 };
    uint32_t level{ 0 };
    bool isParented{ false };
    // shader
    // VAO
    // Texture
    std::weak_ptr<HkNode> parent;
    std::vector<std::shared_ptr<HkNode>> children;
};