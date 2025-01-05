#ifndef ALGORITHMS_H_INCLUDED

#define ALGORITHMS_H_INCLUDED

#include "graph.h"
#include "test-suite-oracle.h"
#include <cstdint>
#include <memory>
#include <set>

typedef std::vector<uint32_t> schedule;

class Algorithm {
public:
  virtual ~Algorithm(void) {};
  virtual std::unique_ptr<Graph> run(const std::vector<uint32_t> &tests,
                                     TestSuiteOracle *oracle) = 0;
};

class PFAST : public Algorithm {
public:
  std::unique_ptr<Graph> run(const std::vector<uint32_t> &tests,
                             TestSuiteOracle *oracle);
};

class PraDet : public Algorithm {
public:
  std::unique_ptr<Graph> run(const std::vector<uint32_t> &tests,
                             TestSuiteOracle *oracle);
};

class MEMFAST : public Algorithm {
public:
  std::unique_ptr<Graph> run(const std::vector<uint32_t> &tests,
                             TestSuiteOracle *oracle);

private:
  class Context {
  public:
    explicit Context(const std::vector<uint32_t> &tests,
                     TestSuiteOracle *oracle);

    std::vector<bool> run_schedule(const schedule &schedule);
    bool runned_schedule(const schedule &schedule);

  private:
    friend class MEMFAST;

    std::set<uint32_t> failed;
    std::unique_ptr<Graph> graph;
    uint32_t max;
    TestSuiteOracle *oracle;
    std::set<schedule> runned;
    std::vector<std::set<schedule>> table;
  };

  void append_failed_tests(Context &ctx, uint32_t rank);
  void extensive_search(Context &ctx, uint32_t rank, uint32_t prefix_len);
  static schedule merge_schedules(const schedule &s1, const schedule &s2);
};

std::unique_ptr<Algorithm> algorithm_factory(const std::string &algo);

#endif
