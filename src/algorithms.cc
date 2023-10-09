#include "algorithms.h"
#include <algorithm>
#include <cstring>
#include <fcntl.h>
#include <fstream>
#include <iterator>
#include <set>
#include <sstream>
#include <sys/file.h>
#include <unistd.h>
#include <utility>

const std::string Metrics::header { "n,test_suite_runs,test_runs,"
                                    "optimal_longest_schedule,longest_schedule,"
                                    "optimal_total_cost,total_cost" };

void Metrics::schedule_metrics(const std::vector<schedule>& schedules)
{
    compute_metrics(schedules, longest_schedule, total_cost);
}

void Metrics::optimal_schedule_metrics(const std::vector<schedule>& schedules)
{
    compute_metrics(schedules, optimal_longest_schedule, optimal_total_cost);
}

void Metrics::compute_metrics(const std::vector<schedule>& schedules, uint64_t& length, uint64_t& cost)
{
    for (const auto& schedule : schedules) {
        length = std::max(length, schedule.size());
        cost += schedule.size();
    }
}

void Metrics::record(const char* fname)
{
    int fd = open(fname, O_WRONLY | O_CREAT, 0644);
    if (fd < 0 || flock(fd, LOCK_EX) != 0) {
        std::cerr << "Failed to write into stats file " << fname
                  << " with error \"" << strerror(errno) << '"' << std::endl;
        return;
    }
    std::ostringstream oss;

    if (lseek(fd, 0, SEEK_END) == 0)
        oss << Metrics::header << std::endl;
    oss << *this << std::endl;
    if (write(fd, oss.str().c_str(), oss.str().size()) == -1)
        std::cerr << "Failed to write into stats file " << fname
                  << " with error \"" << strerror(errno) << '"' << std::endl;

    flock(fd, LOCK_UN);
    close(fd);
}

void GraphResult::output(const char* fname)
{
    if (!fname)
        std::cout << g;
    std::ofstream out { fname };
    out << g;
    out.close();
}

void TableResult::output(const char* fname)
{
    if (!fname) {
        write_schedules(std::cout);
        return;
    }
    std::ofstream out { fname };

    write_schedules(out);
    out.close();
}

void TableResult::write_schedules(std::ostream& os)
{
    for (const auto& schedules : table)
        for (const auto& schedule : schedules.second) {
            if (schedule.size() == 0)
                continue;
            std::copy(schedule.begin(), schedule.end() - 1, std::ostream_iterator<uint32_t>(os, ","));
            os << schedule.back() << std::endl;
        }
}

std::unique_ptr<Result> PFAST::run(const std::vector<uint32_t>& tests, TestSuiteOracle* oracle, Metrics& m)
{
    GraphResult* r { new GraphResult { tests } };

    if (tests.size() == 0)
        return std::unique_ptr<Result> { r };

    for (uint32_t i = 0; i < tests.size() - 1; ++i) {
        std::vector<uint32_t> schedule { tests };
        schedule.erase(schedule.begin() + i);

        m.add_testsuite_run();

        std::vector<bool> results = oracle->run_tests(schedule);
        auto first_false = std::find(results.begin(), results.end(), false);
        m.add_test_runs(first_false - results.begin());
        while (first_false != results.end()) {
            m.add_test_runs(1);
            r->g.add_edge(schedule[first_false - results.begin()], tests[i]);
            if (first_false == results.end() - 1)
                break;
            schedule.erase(schedule.begin() + (first_false - results.begin()));
            results = oracle->run_tests(schedule);
            m.add_testsuite_run();
            first_false = std::find(results.begin(), results.end(), false);
            m.add_test_runs(first_false - results.begin());
        }
    }

    r->g.transitive_reduction();
    m.schedule_metrics(r->g.get_schedules());
    return std::unique_ptr<Result> { r };
}

void PraDet::invert_edge(Graph& g, uint32_t u, uint32_t v)
{
    g.remove_edge(u, v);
    g.add_edge(v, u);
}

std::unique_ptr<Result> PraDet::run(const std::vector<uint32_t>& tests, TestSuiteOracle* oracle, Metrics& m)
{
    GraphResult* r { new GraphResult { tests } };
    std::set<std::pair<uint32_t, uint32_t>> edges {};
    uint32_t tried_edges { 0 };

    if (tests.size() == 0)
        return std::unique_ptr<Result> { r };

    // Build fully connected graph
    for (auto it1 = r->g.begin(); it1 != r->g.end(); ++it1)
        for (auto it2 = std::next(it1); it2 != r->g.end(); ++it2) {
            edges.insert(std::make_pair(it2->first, it1->first));
            r->g.add_edge(it2->first, it1->first);
        }

    auto edge { edges.begin() };
    while (!edges.empty()) {
        // Select one edge from the graph
        invert_edge(r->g, edge->first, edge->second);
        ++tried_edges;
        std::unordered_set<uint32_t> deps { r->g.get_dependencies(edge->second) };
        while (deps.find(edge->second) != deps.end()) {
            if (tried_edges == edges.size()) {
                edge = edges.end();
                break;
            }
            invert_edge(r->g, edge->second, edge->first);
            if (++edge == edges.end())
                edge = edges.begin();
            ++tried_edges;
            invert_edge(r->g, edge->first, edge->second);
            deps = r->g.get_dependencies(edge->second);
        }

        if (edge == edges.end())
            break;

        // Build schedule with the inverted edge
        std::vector<uint32_t> schedule {};
        for (const uint32_t t : tests)
            if (deps.find(t) != deps.end())
                schedule.push_back(t);
        schedule.push_back(edge->second);

        // Run schedule to get results
        std::vector<bool> results = oracle->run_tests(schedule);
        auto first_false = std::find(results.begin(), results.end(), false);

        m.add_test_runs(first_false - results.begin());
        m.add_testsuite_run();

        // Remove the inverted edge and add the original edge back in case
        // some test fails
        r->g.remove_edge(edge->second, edge->first);
        if (first_false != results.end()) {
            m.add_test_runs(1);
            r->g.add_edge(edge->first, edge->second);
        }

        edge = edges.erase(edge);
        tried_edges = 0;
        if (edge == edges.end())
            edge = edges.begin();
    }

    r->g.transitive_reduction();
    m.schedule_metrics(r->g.get_schedules());
    return std::unique_ptr<Result> { r };
}

std::vector<schedule> table_to_schedules(const std::map<uint32_t, std::set<schedule>>& table)
{
    std::vector<schedule> initial_scan, schedules;
    std::unordered_set<uint32_t> covered;

    for (const auto& it : table) {
        for (const auto& s : it.second)
            if (covered.find(s.back()) == covered.end()) {
                covered.insert(s.back());
                initial_scan.push_back(s);
            }
    }

    covered.clear();

    for (auto it = initial_scan.rbegin(); it != initial_scan.rend(); it++) {
        bool to_add { false };
        for (const uint32_t test : *it)
            if (covered.find(test) == covered.end()) {
                to_add = true;
                covered.insert(test);
            }

        if (to_add)
            schedules.push_back(*it);
    }

    return schedules;
}

std::unique_ptr<Result> MEMFAST::run(const std::vector<uint32_t>& tests, TestSuiteOracle* oracle, Metrics& m)
{
    std::set<uint32_t> not_passed {};
    TableResult* r { new TableResult {} };

    if (tests.size() == 0)
        return std::unique_ptr<Result> { r };

    for (const uint32_t t : tests) {
        schedule schedule { { t } };
        if (oracle->run_tests(schedule)[0])
            r->table[1].insert(schedule);
        else
            not_passed.insert(t);
    }

    uint32_t rank { 1 };

    while (!not_passed.empty()) {

        if (r->table.find(rank) == r->table.end())
            goto ex_search;

        for (const auto& seq : r->table[rank]) {
            for (auto test { not_passed.begin() }; test != not_passed.end();) {
                if (seq.back() > *test) {
                    ++test;
                    continue;
                }
                schedule schedule { seq };
                schedule.push_back(*test);
                std::vector<bool> results = oracle->run_tests(schedule);
                auto first_false { std::find(results.begin(), results.end(), false) };

                m.add_test_runs(first_false - results.begin() + (first_false == results.end() ? 0 : 1));
                m.add_testsuite_run();
                if (first_false == results.end()) {
                    r->table[schedule.size()].insert(schedule);
                    test = not_passed.erase(test);
                } else
                    ++test;
            }
        }
    ex_search:
        ++rank;
        auto it { not_passed.begin() };
        if (it == not_passed.end() || *it > rank - 1)
            continue;

        exhaustive_search(not_passed, r->table, oracle, m);
    }

    m.schedule_metrics(table_to_schedules(r->table));
    return std::unique_ptr<Result> { r };
}

void MEMFAST::exhaustive_search(std::set<uint32_t>& not_passed, std::map<uint32_t, std::set<schedule>>& table, TestSuiteOracle* oracle, Metrics& m)
{
    uint32_t target { *not_passed.begin() };

    for (uint32_t len { 2 }; len <= target + 1; ++len) {
        for (uint32_t f { 1 }; f <= len / 2; ++f) {
            auto sets1 { table.find(f) }, sets2 { table.find(len - f) };

            if (sets1 == table.end() || sets2 == table.end())
                continue;

            auto it1 { sets1->second.begin() };

            for (; it1 != sets1->second.end(); ++it1) {
                auto it2 { len - f == f ? std::next(it1) : sets2->second.begin() };
                for (; it2 != sets2->second.end(); ++it2) {
                    schedule seq { *it1 };

                    seq.insert(seq.end(), it2->begin(), it2->end());
                    std::sort(seq.begin(), seq.end());
                    seq.erase(std::unique(seq.begin(), seq.end()), seq.end());
                    table[seq.size()].insert(seq);

                    bool target_passed { false };

                    for (auto t { not_passed.begin() }; t != not_passed.end();) {
                        if (seq.back() > *t) {
                            ++t;
                            continue;
                        }

                        schedule schedule { seq };
                        schedule.push_back(*t);
                        std::vector<bool> results = oracle->run_tests(schedule);
                        auto first_false { std::find(results.begin(), results.end(), false) };

                        m.add_test_runs(first_false - results.begin() + (first_false == results.end() ? 0 : 1));
                        m.add_testsuite_run();
                        if (first_false == results.end()) {
                            target_passed = *t == target;
                            table[schedule.size()].insert(schedule);
                            t = not_passed.erase(t);
                        } else
                            ++t;
                    }

                    if (target_passed)
                        return;
                }
            }
        }
    }
}

std::unique_ptr<Algorithm>
algorithm_factory(const std::string& algo)
{
    if (algo == "pradet")
        return std::unique_ptr<Algorithm> { new PraDet {} };
    else if (algo == "pfast")
        return std::unique_ptr<Algorithm> { new PFAST {} };
    else if (algo == "mem-fast")
        return std::unique_ptr<Algorithm> { new MEMFAST {} };
    std::cerr << algo << " is not a valid method to find dependencies between tests." << std::endl;
    exit(EXIT_FAILURE);
}

std::ostream&
operator<<(std::ostream& os, const Metrics& m)
{
    return os << m.n << ',' << m.test_suite_runs << ',' << m.test_runs << ','
              << m.optimal_longest_schedule << ',' << m.longest_schedule << ','
              << m.optimal_total_cost << ',' << m.total_cost;
}
