#ifndef ALGORITHMS_H_INCLUDED

#define ALGORITHMS_H_INCLUDED

#include "graph.h"
#include "test-suite-oracle.h"
#include <iostream>
#include <memory>
#include <set>

typedef std::vector<uint32_t> schedule;

class Metrics {
public:
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
    void schedule_metrics(const std::vector<schedule>&);
    void optimal_schedule_metrics(const std::vector<schedule>&);
    void record(const char*);

private:
    friend std::ostream& operator<<(std::ostream&, const Metrics&);
    void compute_metrics(const std::vector<schedule>&, uint64_t&, uint64_t&);

    static const std::string header;

    uint64_t n;
    uint64_t test_suite_runs;
    uint64_t test_runs;
    uint64_t optimal_longest_schedule;
    uint64_t longest_schedule;
    uint64_t optimal_total_cost;
    uint64_t total_cost;
};

class Result {
public:
    virtual ~Result() {}
    virtual void output(const char*) = 0;
};

class GraphResult : public Result {
public:
    GraphResult(const std::vector<uint32_t>& tests)
        : g { tests } {};
    virtual ~GraphResult() {}
    void output(const char*);

    Graph g;
};

class TableResult : public Result {
public:
    TableResult()
        : table {}
    {
    }
    virtual ~TableResult() {}
    void output(const char*);

    std::map<uint32_t, std::set<schedule>> table;

private:
    void write_schedules(std::ostream&);
};

class Algorithm {
public:
    virtual ~Algorithm() {};
    virtual std::unique_ptr<Result> run(const std::vector<uint32_t>&, TestSuiteOracle*, Metrics&) = 0;
};

class PFAST : public Algorithm {
public:
    std::unique_ptr<Result> run(const std::vector<uint32_t>&, TestSuiteOracle*, Metrics&);
};

class PraDet : public Algorithm {
public:
    std::unique_ptr<Result> run(const std::vector<uint32_t>&, TestSuiteOracle*, Metrics&);

private:
    void invert_edge(Graph&, uint32_t, uint32_t);
};

class MEMFAST : public Algorithm {
public:
    std::unique_ptr<Result> run(const std::vector<uint32_t>&, TestSuiteOracle*, Metrics&);

private:
    void exhaustive_search(std::set<uint32_t>&, std::map<uint32_t, std::set<schedule>>&, TestSuiteOracle*, Metrics&);
};

std::unique_ptr<Algorithm> algorithm_factory(const std::string&);

std::ostream&
operator<<(std::ostream&, const Metrics&);

#endif
