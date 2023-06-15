#include "base/HkNode.hpp"

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

    // master->pushChildren({ slave1, slave4 });

    slave4->pushChildren({ slave6,slave7 });

    slave1->pushChildren({ slave2 });


    // printf("\n");

    // slave4->removeChildren({ slave4->getId(), slave7->getId() });
    // slave4->removeChildren({ slave6->getId() });
    // slave4->removeChildren({ slave4->getId() });
    HkNode::reParent({ slave2 }, slave4);
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


    const auto val = slave2->getParent();
    if (val.lock())
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