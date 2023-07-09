#include "HkNode.hpp"

namespace hkui
{
HkNode::HkNode(const std::string& name, const std::string& type,
    const std::string& vertPath, const std::string& fragPath)
    : name{ name }, type{ type }, id{ genId() }, renderContext(vertPath, fragPath) {}

/**
 * Desc: Reparents list of nodes to new parent. All ties to old parents will be
 * removed accordingly
 * Returns: Nothing.
*/
void HkNode::reParent(const std::vector<std::shared_ptr<HkNode>>& nodes, const std::shared_ptr<HkNode> newParent)
{
    // TODO: Do not allow reparenting of Node as child of Node child nodes, it doesnt make sense
    // TODO: Do not allow reparenting the root node as child of some other node (the root node has no parent)
    for (const auto& node : nodes)
    {
        const auto& parentOfNode = node->getParent();
        if (const auto& lockedParent = parentOfNode.lock())
        {
            if (node->hasChild(newParent->getId()))
            {
                std::cout << "Cannot reparent parent node " << node << " as child of parent's children "
                    << newParent << "!\n";
                continue;
            }
            //TODO: Make it one call
            lockedParent->removeChildren({ node->getId() });
        }
    }
    newParent->pushChildren(nodes);
}

/**
 * Desc: Removes children Ids from child list of node and also clears
 * reference to parent to nullptr for these nodes.
 * Returns: Nothing.
*/
void HkNode::removeChildren(const std::set<uint32_t>& rmChildrenIds)
{
    std::vector<std::shared_ptr<HkNode>> interestingNodes;
    interestingNodes.reserve(rmChildrenIds.size());
    const auto it = std::remove_if(children.begin(), children.end(),
        [&rmChildrenIds, &interestingNodes](const std::shared_ptr<HkNode>& node)
        {
            const bool shouldRemove = rmChildrenIds.count(node->id);
            if (shouldRemove) interestingNodes.push_back(node);
            return shouldRemove;
        });

    for (const auto& node : interestingNodes)
    {
        node->setParent(nullptr);
    }
    children.erase(it, children.end());
}

/**
 * Desc: Pushes `newChildren` to the list of children of this node and
 * sets the parent of those to be this node IF the nodes are not already
 * parented to another node.
 * Returns: Nothing.
*/
void HkNode::pushChildren(const std::vector<std::shared_ptr<HkNode>>& newChildren)
{
    for (const auto& child : newChildren)
    {
        if (child->isParented || child->getId() == id) continue;
        child->setParent(shared_from_this());
        children.push_back(child);
    }
}

/**
 * Desc: Check if node id exists as child of this node
 * Returns: The node if found, nullptr if not.
*/
std::shared_ptr<HkNode> HkNode::hasChild(uint32_t childId) const
{
    const auto it = std::find_if(children.begin(), children.end(),
        [childId](const auto& childNode)
        {
            return childId == childNode->getId();
        });
    return it != children.end() ? *it : nullptr;
}

/**
 * Desc: Exactly what it says.
 * Returns: Nothing.
*/
void HkNode::printChildren() const
{
    std::cout << "Node " << name.c_str() << " with children:\n";
    for (uint32_t curr = 1; const auto & child : children)
    {
        std::cout << "    -- " << child->getName().c_str();
        if (curr++ != children.size())
            std::cout << "\n";
    }
}

/**
 * Desc: Prints a breath first search representation of the nodes with each new
 * line representing a new level.
 * Returns: Nothing.
*/
void HkNode::printBfs() const
{
    std::queue<std::shared_ptr<const HkNode>> queue;
    std::set<uint32_t> idsVisited;
    queue.push(shared_from_this());

    std::cout << shared_from_this()->name.c_str() << "-" << shared_from_this()->level;

    uint32_t prevLevel = 0;
    while (!queue.empty())
    {
        const auto node = queue.front();
        queue.pop();
        idsVisited.insert(node->id);

        for (const auto& child : node->children)
        {
            if (idsVisited.count(child->id))
            {
                return;
            }

            if (prevLevel != child->level)
            {
                std::cout << '\n';
                prevLevel = child->level;
            }
            std::cout << child->name.c_str() << "-" << child->level << " ";

            queue.push(child);
        }
    }
}

/**
 * Desc: Auxiliary function for HkNode::printTree.
 * Returns: Nothing.
*/
void HkNode::printTreeAux(int level) const
{
    for (int x = 0; x < level - 1; x++)
        std::cout << "    ";

    if (level == 0)
        std::cout << "| " << this << '\n';
    else
        std::cout << "----| " << this << '\n';

    for (const auto& node : children)
    {
        node->printTreeAux(level + 1);
    }
}

/**
 * Desc: Prints a tree representation of the nodes.
 * Returns: Nothing.
*/
void HkNode::printTree() const { printTreeAux(0); }

/**
 * Desc: Exactly what it says plus increments the level of the node
 * to reflect parent.level+1
 * Returns: Nothing.
*/
void HkNode::setParent(const std::shared_ptr<HkNode>& newParent)
{
    parent = newParent;
    level = newParent != nullptr ? newParent->level + 1 : 0;
    isParented = newParent != nullptr ? true : false;
}

/**
 * Desc: Generate node id
 * Returns: New node id.
*/
uint32_t HkNode::genId()
{
    static uint32_t currentId = 0;
    return currentId++;
}

/*Overloads*/
std::ostream& operator<<(std::ostream& os, const std::shared_ptr<HkNode>& node)
{
    os << "(" << node->type << ": " << node->name << ", Id = " << node->id << ")";
    return os;
}

std::ostream& operator<<(std::ostream& os, const HkNode& node)
{
    os << "(" << node.type << ": " << node.name << ", Id = " << node.id << ")";
    return os;
}
std::ostream& operator<<(std::ostream& os, const HkNode* node)
{
    os << "(" << node->type << ": " << node->name << ", Id = " << node->id << ")";
    return os;
}
} // hkui