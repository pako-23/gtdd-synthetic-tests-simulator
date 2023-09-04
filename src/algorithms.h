#ifndef ALGORITHMS_H_INCLUDED

#define ALGORITHMS_H_INCLUDED

#include "graph.h"
#include "test-suite-oracle.h"
#include <iostream>

typedef std::vector<uint32_t> schedule;
typedef std::vector<schedule> schedule_list;

struct Metrics {
    static std::string& get_header(void);

    Metrics(uint64_t n)
        : n { n }
        , test_suite_runs { 0 }
        , test_runs { 0 }
        , optimal_longest_schedule { 0 }
        , longest_schedule { 0 }
        , optimal_total_cost { 0 }
        , total_cost { 0 }
    {
    }

    inline void add_testsuite_run() { ++test_suite_runs; }
    inline void add_test_runs(uint64_t n) { test_runs += n; };
    void schedule_metrics(const schedule_list&);
    void optimal_schedule_metrics(const schedule_list&);

private:
    friend std::ostream& operator<<(std::ostream&, const Metrics&);
    void compute_metrics(const schedule_list&, uint64_t&, uint64_t&);

    uint64_t n;
    uint64_t test_suite_runs;
    uint64_t test_runs;
    uint64_t optimal_longest_schedule;
    uint64_t longest_schedule;
    uint64_t optimal_total_cost;
    uint64_t total_cost;
};

struct Algorithm {
    Algorithm(const std::string& file)
        : results_file { file }
    {
    }
    virtual ~Algorithm() {};
    virtual void run(const std::vector<uint32_t>&, TestSuiteOracle*, Metrics&) = 0;

protected:
    std::string results_file;
};

struct ExLinear : public Algorithm {
    ExLinear(const std::string& file)
        : Algorithm { file }
    {
    }
    void run(const std::vector<uint32_t>&, TestSuiteOracle*, Metrics&);
};

struct Pradet : public Algorithm {
    Pradet(const std::string& file)
        : Algorithm { file }
    {
    }
    void run(const std::vector<uint32_t>&, TestSuiteOracle*, Metrics&);

private:
    void invert_edge(Graph&, uint32_t, uint32_t);
};

struct BigTable : public Algorithm {
    BigTable(const std::string& file)
        : Algorithm { file }
    {
    }
    void run(const std::vector<uint32_t>&, TestSuiteOracle*, Metrics&);
};

Algorithm*
algorithm_factory(const std::string&, const std::string&);

std::ostream&
operator<<(std::ostream&, const Metrics&);

#endif
