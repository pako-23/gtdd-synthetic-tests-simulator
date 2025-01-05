#ifndef TEST_SUITE_ORACLE_H_INCLUDED
#define TEST_SUITE_ORACLE_H_INCLUDED

#include "graph.h"
#include <cstdint>
#include <vector>

class TestSuiteOracle {
public:
  virtual ~TestSuiteOracle(void) {}
  virtual std::vector<bool> run_tests(const std::vector<uint32_t> &tests) = 0;
  virtual std::vector<uint32_t> tests(void) const = 0;
};

class GraphGeneratorParams {
public:
  std::string generator_type;
  double probability;
  uint32_t min_out;
  uint32_t max_out;
};

class DirectDependenciesOracle : public TestSuiteOracle {
public:
  DirectDependenciesOracle(const std::vector<uint32_t> &nodes,
                           const GraphGeneratorParams &params);
  DirectDependenciesOracle(const Graph &g);
  ~DirectDependenciesOracle(void) override {}

  std::vector<uint32_t> tests(void) const override;
  std::vector<bool> run_tests(const std::vector<uint32_t> &tests) override;
  inline const Graph &get_graph(void) const { return graph; }
  inline uint64_t get_test_suite_runs(void) const { return test_suite_runs; }
  inline uint64_t get_test_runs(void) const { return test_runs; }

private:
  Graph graph;
  uint64_t test_suite_runs;
  uint64_t test_runs;
};

#endif
