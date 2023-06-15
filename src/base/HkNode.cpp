#include "HkNode.hpp"

HkNode::HkNode(const std::string& name)
    : name{ name }, id{ getId() } {}

/**
 * Desc: Reparents list of nodes to new parent. All ties to old parents will be
 * removed accordingly
 * Returns: Nothing.
*/
void HkNode::reParent(const std::vector<std::shared_ptr<HkNode>>& nodes, const std::shared_ptr<HkNode> newParent)
{
    for (const auto& node : nodes)
    {
        const auto& parentOfNode = node->getParent();
        if (const auto& lockedParent = parentOfNode.lock())
        {
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
        printf("Prima data aici %s\n", node->getName().c_str());
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
        if (child->isParented) continue;
        printf("Intra pe aici\n");
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
    printf("Node %s with children:\n", name.c_str());
    for (const auto& child : children)
    {
        printf("    -- %s\n", child->name.c_str());
    }
}

/**
 * Desc: Prints a breath first search representation of the nodes with each new
 * line representing a new level.
 * Returns: Nothing.
*/
void HkNode::printBfs(const std::shared_ptr<HkNode> srcNode) const
{
    std::queue<std::shared_ptr<HkNode>> queue;
    std::set<uint32_t> idsVisited;
    queue.push(srcNode);

    printf("%s-%d", srcNode->name.c_str(), srcNode->level);

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
                printf("\n");
                prevLevel = child->level;
            }
            printf("%s-%d    ", child->name.c_str(), child->level);
            queue.push(child);
        }
    }
    printf("\n");
}

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



