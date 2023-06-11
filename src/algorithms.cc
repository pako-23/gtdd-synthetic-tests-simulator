#include "algorithms.h"
#include <algorithm>
#include <cstring>
#include <utility>
#include <set>
#include <fstream>
#include "./utils.h"


Metrics *ExLinear::run(const std::vector<uint32_t>& tests, TestSuiteOracle *oracle)
{
    Metrics *r{new Metrics{0, 0}};
    Graph g{tests};

    for (uint32_t i = 0; i < tests.size(); ++i) {
        std::vector<uint32_t> schedule{tests};
        schedule.erase(schedule.begin() + i);

        ++(r->test_suite_runs);
        std::vector<bool> results = oracle->run_tests(schedule);
        auto first_false = std::find(results.begin(), results.end(), false);
        r->tests_run += first_false - results.begin();
        while (first_false != results.end()) {
            g.add_edge(schedule[first_false - results.begin()], tests[i]);
            schedule.erase(schedule.begin() + (first_false - results.begin()));
            results = oracle->run_tests(schedule);
            ++(r->test_suite_runs);
            first_false = std::find(results.begin(), results.end(), false);
            r->tests_run += first_false - results.begin();
        }
    }

    g.transitive_reduction();
    std::ofstream out{results_file};
    out << g;
    out.close();

    return r;
}


void Pradet::invert_edge(Graph& g, uint32_t u, uint32_t v)
{
    g.remove_edge(u, v);
    g.add_edge(v, u);
}

Metrics *Pradet::run(const std::vector<uint32_t>& tests, TestSuiteOracle *oracle)
{
    Metrics *r{new Metrics{0, 0}};

    Graph g{tests};
    std::set<std::pair<uint32_t, uint32_t>> edges{};
    uint32_t tried_edges{0};

    // Build fully connected graph
    for (auto it1 = g.begin(); it1 != g.end(); ++it1)
        for (auto it2 = std::next(it1); it2 != g.end(); ++it2) {
            edges.insert(std::make_pair(it2->first, it1->first));
            g.add_edge(it2->first, it1->first);
        }

    auto edge{edges.begin()};
    while (!edges.empty()) {

        // Select one edge from the graph
        invert_edge(g, edge->first, edge->second);
        ++tried_edges;
        std::unordered_set<uint32_t> deps{g.get_dependencies(edge->second)};
        while (deps.find(edge->second) != deps.end()) {
            if (tried_edges == edges.size()) {
                edge = edges.end();
                break;
            }
            invert_edge(g, edge->second, edge->first);
            if (++edge == edges.end()) edge = edges.begin();
            ++tried_edges;
            invert_edge(g, edge->first, edge->second);
            deps = g.get_dependencies(edge->second);
        }

        if (edge == edges.end()) break;

        // Build schedule with the inverted edge
        std::vector<uint32_t> schedule{};
        for (const uint32_t t : tests)
            if (deps.find(t) != deps.end())
                schedule.push_back(t);
        schedule.push_back(edge->second);

        // Run schedule to get results
        std::vector<bool> results = oracle->run_tests(schedule);
        auto first_false = std::find(results.begin(), results.end(), false);
        r->tests_run += first_false - results.begin();
        ++(r->test_suite_runs);

        // Remove the inverted edge and add the original edge back in case
        // some test fails
        g.remove_edge(edge->second, edge->first);
        if (first_false != results.end())
            g.add_edge(edge->first, edge->second);

        edge = edges.erase(edge);
        tried_edges = 0;
        if (edge == edges.end()) edge = edges.begin();
    }

    g.transitive_reduction();
    std::ofstream out{results_file};
    out << g;
    out.close();

    return r;
}

Metrics *BigTable::run(const std::vector<uint32_t>& tests, TestSuiteOracle *oracle)
{
    typedef std::vector<uint32_t> schedule;
    typedef std::vector<schedule> schedule_list;

    Metrics *r{new Metrics{0, 0}};
    std::unordered_set<uint32_t> not_passed{};
    std::map<uint32_t, schedule_list> table;
    std::set<schedule> working_schedules{};

    for (const uint32_t t : tests) {
        schedule schedule{{t}};
        if (oracle->run_tests(schedule)[0]) {
            table[1].push_back(schedule);
            working_schedules.insert(schedule);
        } else not_passed.insert(t);
    }


    for (uint32_t rank{2};  rank <= tests.size(); ++rank) {

        for (auto test = not_passed.begin(); test != not_passed.end();) {
            bool test_passed{false};

            for (const auto& seq : table[rank-1]) {
                if (seq.back() > *test) continue;
                schedule schedule{seq};
                schedule.push_back(*test);
                std::vector<bool> results = oracle->run_tests(schedule);
                auto first_false{std::find(results.begin(), results.end(), false)};
                r->tests_run += first_false - results.begin();
                ++(r->test_suite_runs);
                if (first_false == results.end()) {
                    working_schedules.insert(schedule);
                    table[rank].push_back(schedule);
                    test = not_passed.erase(test);
                    test_passed = true;
                    break;
                }
            }

            if (test_passed) continue;

            if (*test == rank - 1) {
                for (auto base = working_schedules.begin(); base != working_schedules.end(); ++base) {
                    for (auto it = std::next(base); it != working_schedules.end(); ++it) {
                        schedule schedule{*base};
                        schedule.insert(schedule.end(), it->begin(), it->end());
                        std::sort(schedule.begin(), schedule.end());
                        if (schedule.back() > *test) continue;
                        schedule.erase(std::unique(schedule.begin(), schedule.end()), schedule.end());
                        table[schedule.size()-1].push_back(schedule);
                        working_schedules.insert(schedule);
                        if (schedule.back() < *test) {
                            // Check if prefix makes pass our test
                            schedule.push_back(*test);
                            std::vector<bool> results = oracle->run_tests(schedule);
                            auto first_false{std::find(results.begin(), results.end(), false)};
                            r->tests_run += first_false - results.begin();
                            ++(r->test_suite_runs);
                            if (first_false == results.end()) {
                                table[schedule.size()-1].push_back(schedule);
                                working_schedules.insert(schedule);
                                test = not_passed.erase(test);
                                test_passed = true;
                                break;
                            }
                        }
                    }
                    if (test_passed) break;
                }
                continue;
            }

            ++test;
        }
    }

    return r;
}

Algorithm *algorithm_factory(const std::string& algo, const std::string& file)
{
    if (algo == "pradet")
        return new Pradet{file};
    else if (algo == "ex-linear")
        return new ExLinear{file};
    else if (algo == "big-table")
        return new BigTable{file};
    return NULL;
}
