#ifndef TEST_SUITE_ORACLE_H_INCLUDED
#define TEST_SUITE_ORACLE_H_INCLUDED

#include "graph.h"
#include <cstdint>
#include <memory>
#include <vector>

class TestSuiteOracle {
public:
    virtual ~TestSuiteOracle() {}
    virtual std::vector<bool> run_tests(const std::vector<uint32_t>&) = 0;
    virtual std::vector<uint32_t> tests() const = 0;
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
    DirectDependenciesOracle(const std::vector<uint32_t>&,
        const GraphGeneratorParams&);
    DirectDependenciesOracle(const Graph&);
    ~DirectDependenciesOracle() override {}

    std::vector<uint32_t> tests() const;
    std::vector<bool> run_tests(const std::vector<uint32_t>&);
    inline const Graph& get_graph() const { return graph; }

private:
    Graph graph;
};

#endif
