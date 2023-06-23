#include "hkTestFw/HkTestBase.hpp"
#include "tests/base/HkNode.test.hpp"

int main()
{
    (std::make_unique<HkNodeTCS>())->runTests();
}