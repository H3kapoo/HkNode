#pragma once

#include "../../hkTestFw/HkTestBase.hpp"
#include "../../../src/base/HkNode.hpp" // class to be tested (sut)

#include <utility>

class HkNodeTCShould : public IHkTCS<HkNodeTCShould>
{
public:
    void setup()
    {
        /*Inits*/
        node0 = std::make_shared<HkNode>("node0");
        node1 = std::make_shared<HkNode>("node1");
        node2 = std::make_shared<HkNode>("node2");
        node3 = std::make_shared<HkNode>("node3");
        node4 = std::make_shared<HkNode>("node4");
        node5 = std::make_shared<HkNode>("node5");
        node6 = std::make_shared<HkNode>("node6");
        node7 = std::make_shared<HkNode>("node7");
    }

private:
    std::shared_ptr<HkNode> node0, node1, node2, node3, node4, node5, node6, node7;

private:
    void returnCorrectAmountOfNodes();
    void outputCorectBFSStructure();
    void reparentChildrenCorrectly();
    void checkToHaveCorrectChildren();

    DEFINE_SUITE(HkNodeTCShould,
        // TEST_DECLARE(HkNodeTCShould, returnCorrectAmountOfNodes)
        // TEST_DECLARE(HkNodeTCShould, outputCorectBFSStructure)
        // TEST_DECLARE(HkNodeTCShould, reparentChildrenCorrectly)
        TEST_DECLARE(HkNodeTCShould, checkToHaveCorrectChildren)
    )
};
