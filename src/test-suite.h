#ifndef __TEST_SUITE_H_INCLUDED__
#define __TEST_SUITE_H_INCLUDED__

#include <cstdint>
#include <vector>

struct TestSuite
{
    TestSuite(uint32_t);
    virtual ~TestSuite() { };

    virtual std::vector<uint32_t> generate_tests(void) =0;
protected:
    uint32_t n;
};

struct ListTestSuite : public TestSuite
{
    ListTestSuite(uint32_t);

    std::vector<uint32_t> generate_tests(void);
};

#endif
