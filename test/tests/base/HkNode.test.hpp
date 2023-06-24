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
    void test4();
    DEFINE_SUITE(HkNodeTCS,
        TEST_DECLARE(HkNodeTCS, test1)
        TEST_DECLARE(HkNodeTCS, test2)
        TEST_DECLARE(HkNodeTCS, test3)
        TEST_DECLARE(HkNodeTCS, test4)
    )
};

/**
 * declara prin macro TEST_F(clasa, numefunc) => class numeFunc : public clasa {..}
 * ^ testu inheriteaza toate functiile din fixture asa, incepe clean mereu
 * apoi e un program care scaneaza fiecare .cpp file cu TEST_F expandat, le culege pe toate
 * apoi intr un test_main.cpp le instanteaza one by one
 * test1 a;
 * a->testBody();
 * ...la fel pt restu de clase pe care le gaseste
*/