#ifndef TEST_SUITE_H_INCLUDED
#define TEST_SUITE_H_INCLUDED

#include <cstdint>
#include <vector>

class TestSuite {
public:
  TestSuite(uint32_t);
  virtual ~TestSuite(){};

  virtual std::vector<uint32_t> generate_tests(void) = 0;

protected:
  uint32_t n;
};

class ListTestSuite : public TestSuite {
public:
  ListTestSuite(uint32_t);

  std::vector<uint32_t> generate_tests(void);
};

#endif
