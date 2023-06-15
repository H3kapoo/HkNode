#include <iostream>
#include <vector>
#include <algorithm>
#include <memory>
#include <queue>
#include <set>
#include <utility>

#define GET(typ, n, x) typ get##n() { return x;}

uint32_t idGiver = 0;

class HkNode : public std::enable_shared_from_this<HkNode>
{
public:
    HkNode(const std::string& name)
        : name{ name }, id{ idGiver++ } {}

    /**
     * Desc: Removes children Ids from child list of node and also clears
     * reference to parent to nullptr for these nodes.
     * Returns: Nothing.
    */
    void removeChildren(const std::set<uint32_t>& rmChildrenIds)
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
            node->isParented = false;
            node->setParent(nullptr);
        }
        children.erase(it, children.end());
    }

    /**
     * Desc: Reparents list of nodes to new parent. All ties to old parents will be
     * removed accordingly
     * Returns: Nothing.
    */
    static void reParent(const std::vector<std::shared_ptr<HkNode>>& nodes, const std::shared_ptr<HkNode> newParent)
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
     * Desc: Pushes `newChildren` to the list of children of this node and
     * sets the parent of those to be this node IF the nodes are not already
     * parented to another node.
     * Returns: Nothing.
    */
    void pushChildren(const std::vector<std::shared_ptr<HkNode>>& newChildren)
    {
        for (const auto& child : newChildren)
        {
            if (child->isParented) continue;
            child->setParent(shared_from_this());
            children.push_back(child);
        }
    }

    /**
     * Desc: Check if node id exists as child of this node
     * Returns: The node if found, nullptr if not.
    */
    std::shared_ptr<HkNode> hasChild(uint32_t childId) const
    {
        const auto it = std::find_if(children.begin(), children.end(),
            [childId](auto id)
            {
                return childId == true;
            });
        return it != children.end() ? *it : nullptr;
    }

    /**
     * Desc: Exactly what it says.
     * Returns: Nothing.
    */
    void printChildren() const
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
    void printBfs(const std::shared_ptr<HkNode> srcNode) const
    {
        std::queue<std::shared_ptr<HkNode>> queue;
        std::set<uint32_t> idsVisited;
        queue.push(srcNode);

        printf("%s-%d", srcNode->name.c_str(), srcNode->level);

        int prevLevel = 0;
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

public:
    GET(std::string, Name, name);
    GET(uint32_t, Id, id);
    GET(uint32_t, Level, level);
    GET(std::weak_ptr<HkNode>, Parent, parent);
private:
    /**
     * Desc: Exactly what it says plus increments the level of the node
     * to reflect parent.level+1
     * Returns: Nothing.
    */
    void setParent(const std::shared_ptr<HkNode>& newParent)
    {
        parent = newParent;
        level = newParent != nullptr ? newParent->level + 1 : 0;
        isParented = true;
    }

    std::string name{"UNSET_NODE"};
    uint32_t id{ 0 };
    uint32_t level{ 0 };
    bool isParented{ false };
    // shader
    // VAO
    // Texture
    std::weak_ptr<HkNode> parent;
    std::vector<std::shared_ptr<HkNode>> children;
};

int main()
{
    std::shared_ptr<HkNode> master, slave1, slave2, slave3, slave4, slave5, slave6, slave7;
    master = std::make_shared<HkNode>("Master");
    slave1 = std::make_shared<HkNode>("Slave1");
    slave2 = std::make_shared<HkNode>("Slave2");
    slave3 = std::make_shared<HkNode>("Slave3");
    slave4 = std::make_shared<HkNode>("Slave4");
    slave5 = std::make_shared<HkNode>("Slave5");
    slave6 = std::make_shared<HkNode>("Slave6");
    slave7 = std::make_shared<HkNode>("Slave7");

    master->pushChildren({ slave1, slave4 });

    slave4->pushChildren({ slave6,slave7 });

    slave1->pushChildren({ slave2, slave3, slave6 });


    // printf("\n");

    // slave4->removeChildren({ slave4->getId(), slave7->getId() });
    // slave4->removeChildren({ slave6->getId() });
    // slave4->removeChildren({ slave4->getId() });
    HkNode::reParent({ slave2 }, slave4);
    // master->printBfs(master);
    master->printChildren();
    slave1->printChildren();
    slave2->printChildren();
    slave3->printChildren();
    slave4->printChildren();
    slave5->printChildren();
    slave6->printChildren();
    slave7->printChildren();


    const auto val = master->hasChild(slave1->getId());
    if (val)
    {
        printf("da\n");
    }
    return 0;
}

/*
                    Master
           Salve1             Slave4
    Slave2 Slave3 Slave5    S6      S7


*/