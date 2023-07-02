#include "HkNode.test.hpp"

void HkNodeTCShould::returnCorrectAmountOfNodes()
{
    node0->pushChildren({ node1, node2 });
    EXPECT_EQ(node0->getChildren().size(), 2u);

    node0->pushChildren({ node1, node2, node3 });
    EXPECT_EQ(node0->getChildren().size(), 3u);

    node0->removeChildren({ node1->getId(), node3->getId() });
    EXPECT_EQ(node0->getChildren().size(), 1u);
}

void HkNodeTCShould::outputCorectBFSStructure()
{
    node0->pushChildren({ node1, node2, node3 });
    node1->pushChildren({ node2, node3, node4 });

    EXPECT_OUTPUT(node0->printBfs(),
        "node0-0\n"
        "node1-1 node2-1 node3-1 \n"
        "node4-2 ");
}

void HkNodeTCShould::reparentChildrenCorrectly()
{
    node0->pushChildren({ node1 });
    HkNode::reParent({ node1 }, node2);

    EXPECT_EQ(node2->getChildren().size(), 1u);
    EXPECT_EQ(node0->getChildren().size(), 0u);

    EXPECT_OUTPUT(node0->printBfs(), "node0-0");
    EXPECT_OUTPUT(node2->printBfs(),
        "node2-0\n"
        "node1-1 ");
}

void HkNodeTCShould::checkToHaveCorrectChildren()
{
    node3->pushChildren({ node1, node2, node3, node4 });
    // EXPECT_EQ(0, 1);
    // if (node3->hasChild(node3->getId()))
    // {
    //     std::cout << "ceva ok\n";
    // }

    // EXPECT_EQ(nullptr, nullptr); //null null
    // EXPECT_EQ(nullptr, 3); //null null

    // TODO: More work needs to be done on the test FW

}
