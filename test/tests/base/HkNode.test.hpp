#pragma once

#include "../../hkTestFw/HkTestBase.hpp"
#include "../../../src/base/HkNode.hpp" // class to be tested (sut)

class HkNodeTCS : public IHkTCS<HkNodeTCS>
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
    void test1();
    void test2();
    void test3();
    DEFINE_SUITE(HkNodeTCS,
        TEST_DECLARE(HkNodeTCS, test1)
        TEST_DECLARE(HkNodeTCS, test2)
        TEST_DECLARE(HkNodeTCS, test3)
    )
};