#ifndef __ALGORITHMS_H_INCLUDED__

#define __ALGORITHMS_H_INCLUDED__

#include "graph.h"
#include "test-suite-oracle.h"


struct Metrics
{
    uint64_t test_suite_runs;
    uint64_t tests_run;
};


struct Algorithm
{
    Algorithm(const std::string& file) : results_file{file} { }
    virtual ~Algorithm() {};
    virtual Metrics *run(const std::vector<uint32_t>&, TestSuiteOracle *) =0;
protected:
    std::string results_file;
};

struct ExLinear : public Algorithm
{
    ExLinear(const std::string& file) : Algorithm{file} { }
    Metrics *run(const std::vector<uint32_t>&, TestSuiteOracle *);
};

struct Pradet : public Algorithm
{
    Pradet(const std::string& file) : Algorithm{file} { }
    Metrics *run(const std::vector<uint32_t>&, TestSuiteOracle *);
private:
    void invert_edge(Graph&, uint32_t, uint32_t);
};

struct BigTable : public Algorithm
{
    BigTable(const std::string& file) : Algorithm{file} { }
    Metrics *run(const std::vector<uint32_t>&, TestSuiteOracle *);
};

Algorithm *algorithm_factory(const std::string&, const std::string&);

#endif
