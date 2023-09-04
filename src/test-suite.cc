#include "test-suite.h"
#include <iostream>

TestSuite::TestSuite(uint32_t nodes)
    : n { nodes }
{
}

ListTestSuite::ListTestSuite(uint32_t nodes)
    : TestSuite { nodes }
{
}

std::vector<uint32_t>
ListTestSuite::generate_tests()
{
    std::vector<uint32_t> v(n);
    for (uint32_t i = 0; i < n; ++i)
        v[i] = i;
    return v;
}
