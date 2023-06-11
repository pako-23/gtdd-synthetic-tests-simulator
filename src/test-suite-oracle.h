#ifndef __TEST_SUITE_ORACLE_H_INCLUDED__

#define __TEST_SUITE_ORACLE_H_INCLUDED__

#include "graph.h"
#include <cstdint>
#include <vector>

struct TestSuiteOracle
{
    virtual ~TestSuiteOracle() {}
    virtual std::vector<bool> run_tests(const std::vector<uint32_t>&) =0;
};

struct GraphGeneratorParams
{
    std::string generator_type;
    double probability;
    uint32_t min_out;
    uint32_t max_out;
};

struct DirectDependenciesOracle : public TestSuiteOracle
{
    DirectDependenciesOracle(const std::vector<uint32_t>&, const GraphGeneratorParams&);
    ~DirectDependenciesOracle() override;

    std::vector<bool> run_tests(const std::vector<uint32_t>&);
    inline const GraphGenerator *get_generator() const { return graph_generator; };
private:
    GraphGenerator *graph_generator;
};

#endif
