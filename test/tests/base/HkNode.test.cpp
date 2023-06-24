#include "HkNode.test.hpp"

void HkNodeTCS::test1() {

    node0->pushChildren({ node1, node2 });

    EXPECT_EQ(node0->getChildren().size(), 2u);

    node0->pushChildren({ node1, node2, node3 });

    EXPECT_EQ(node0->getChildren().size(), 3u);

    EXPECT_NOT_EQ("ceva", "cevaa");

}

void HkNodeTCS::test2() {

    node0->pushChildren({ node1, node2 });

    EXPECT_EQ(node0->getChildren().size(), 1u);
}

void HkNodeTCS::test3() {

    node0->pushChildren({ node1, node2 });
    EXPECT_OUTPUT(node0->printChildren(), "Node node0 with children:\n"
        "    -- node1\n"
        "    -- node22");
}

void HkNodeTCS::test4()
{
    EXPECT_LT(node0->getChildren().size() + 3, 2u);
}

