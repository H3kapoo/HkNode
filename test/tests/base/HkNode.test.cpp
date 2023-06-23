#include "HkNode.test.hpp"

void HkNodeTCS::test1() {

    node0->pushChildren({ node1, node2 });

    EXPECT_EQ(node0->getChildren().size(), 2u);

    node0->pushChildren({ node1, node2, node3 });

    EXPECT_EQ(node0->getChildren().size(), 3u);
}

void HkNodeTCS::test2() {

    node0->pushChildren({ node1, node2 });

    EXPECT_NOT_EQ(node0->getChildren().size(), 2u);
}

void HkNodeTCS::test3() {

    // node0->pushChildren({ node1, node2 });

    EXPECT_EQ(node0->getChildren().size(), 1u);
}
