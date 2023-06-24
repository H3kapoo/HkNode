#include "hkTestFw/HkTestBase.hpp"
#include "tests/base/HkNode.test.hpp"

#include <vector>

int main()
{
    (std::make_unique<HkNodeTCS>())->runTests();
}