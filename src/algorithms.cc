#include "algorithms.h"
#include "graph.h"
#include "test-suite-oracle.h"
#include <algorithm>
#include <cstdint>
#include <cstring>
#include <fcntl.h>
#include <iostream>
#include <iterator>
#include <memory>
#include <ostream>
#include <set>
#include <unistd.h>
#include <utility>

std::unique_ptr<Graph> PFAST::run(const std::vector<uint32_t> &tests,
                                  TestSuiteOracle *oracle) {
  std::unique_ptr<Graph> r{std::make_unique<Graph>(tests)};

  if (tests.size() == 0)
    return r;

  for (uint32_t i = 0; i < tests.size() - 1; ++i) {
    std::vector<uint32_t> schedule{tests};
    schedule.erase(schedule.begin() + i);

    std::vector<bool> results = oracle->run_tests(schedule);
    auto first_false = std::find(results.begin(), results.end(), false);

    while (first_false != results.end()) {
      r->add_edge(schedule[first_false - results.begin()], tests[i]);
      if (first_false == results.end() - 1)
        break;
      schedule.erase(schedule.begin() + (first_false - results.begin()));
      results = oracle->run_tests(schedule);
      first_false = std::find(results.begin(), results.end(), false);
    }
  }

  r->transitive_reduction();

  return r;
}

std::unique_ptr<Graph> PraDet::run(const std::vector<uint32_t> &tests,
                                   TestSuiteOracle *oracle) {
  std::unique_ptr<Graph> r{std::make_unique<Graph>(tests)};
  std::set<std::pair<uint32_t, uint32_t>> edges{};
  uint32_t tried_edges{0};

  if (tests.size() == 0)
    return r;

  // Build fully connected graph
  for (auto it1 = r->begin(); it1 != r->end(); ++it1)
    for (auto it2 = std::next(it1); it2 != r->end(); ++it2) {
      edges.insert(std::make_pair(it2->first, it1->first));
      r->add_edge(it2->first, it1->first);
    }

  auto edge{edges.begin()};
  while (!edges.empty()) {
    // Select one edge from the graph
    r->invert_edge(edge->first, edge->second);
    ++tried_edges;
    std::unordered_set<uint32_t> deps{r->get_dependencies(edge->second)};
    while (deps.find(edge->second) != deps.end()) {
      r->invert_edge(edge->second, edge->first);
      if (tried_edges == edges.size()) {
        edge = edges.end();
        break;
      }
      if (++edge == edges.end())
        edge = edges.begin();
      r->invert_edge(edge->first, edge->second);
      ++tried_edges;
      deps = r->get_dependencies(edge->second);
    }

    if (edge == edges.end())
      break;

    // Build schedule with the inverted edge
    std::vector<uint32_t> schedule{};
    for (const uint32_t t : tests)
      if (deps.find(t) != deps.end())
        schedule.push_back(t);
    schedule.push_back(edge->second);

    // Run schedule to get results
    std::vector<bool> results = oracle->run_tests(schedule);
    auto first_false = std::find(results.begin(), results.end(), false);

    // Remove the inverted edge and add the original edge back in case
    // some test fails
    r->remove_edge(edge->second, edge->first);
    if (first_false != results.end())
      r->add_edge(edge->first, edge->second);

    edge = edges.erase(edge);
    tried_edges = 0;
    if (edge == edges.end())
      edge = edges.begin();
  }

  r->transitive_reduction();

  return r;
}

MEMFAST::Context::Context(const std::vector<uint32_t> &tests,
                          TestSuiteOracle *oracle)
    : failed{}, graph{std::make_unique<Graph>(tests)}, max{0}, oracle{oracle},
      runned{}, table{tests.size()} {
  for (const uint32_t t : tests) {
    schedule schedule{t};

    if (!run_schedule(schedule)[0])
      failed.insert(t);
  }
}

std::vector<bool> MEMFAST::Context::run_schedule(const schedule &schedule) {
  auto it = runned.insert(schedule);

  std::vector<bool> result = oracle->run_tests(schedule);

  if (it.second && result[schedule.size() - 1]) {
    table[schedule.size() - 1].insert(schedule);
    if (schedule.size() - 1 > max)
      max = schedule.size() - 1;
  }

  return result;
}

bool MEMFAST::Context::runned_schedule(const schedule &schedule) {
  return runned.find(schedule) != runned.end();
}

void MEMFAST::append_failed_tests(Context &ctx, uint32_t rank) {
  for (const schedule &seq : ctx.table[rank - 1]) {
    schedule schedule(seq);

    for (auto test = ctx.failed.begin(); test != ctx.failed.end();) {
      if (schedule.back() > *test) {
        ++test;
        continue;
      }

      schedule.push_back(*test);
      std::vector<bool> results = ctx.run_schedule(schedule);
      schedule.pop_back();

      if (results.back()) {
        ctx.graph->add_edge(*test, schedule.back());
        test = ctx.failed.erase(test);
      } else {
        ++test;
      }
    }
  }
}

void MEMFAST::extensive_search(Context &ctx, uint32_t rank,
                               uint32_t prefix_len) {
  std::set<schedule> passing;

  for (uint32_t base = 1; base <= prefix_len / 2; ++base) {

    for (const schedule &s1 : ctx.table[base - 1]) {

      for (uint32_t idx = prefix_len - base - 1; idx <= ctx.max; ++idx) {

        for (const schedule &s2 : ctx.table[idx]) {
          schedule sched = merge_schedules(s1, s2);
          if (sched.empty() || sched.size() > prefix_len)
            continue;

          ctx.run_schedule(sched);

          for (auto test = ctx.failed.begin(); test != ctx.failed.end();) {
            if (sched.back() > *test) {
              ++test;
              continue;
            }

            sched.push_back(*test);

            if (ctx.runned_schedule(sched)) {
              sched.pop_back();
              ++test;
              continue;
            }

            std::vector<bool> results = ctx.run_schedule(sched);

            if (results.back()) {
              for (uint32_t i = 0; i < sched.size() - 1; ++i)
                ctx.graph->add_edge(*test, sched[i]);

              if (sched.size() <= rank)
                passing.insert(sched);

              test = ctx.failed.erase(test);
            } else {
              ++test;
            }

            sched.pop_back();
          }
        }
      }
    }
  }

  while (!passing.empty()) {
    std::set<schedule> new_passing;

    for (const auto &item : passing) {
      schedule sched{item};

      for (auto test = ctx.failed.begin(); test != ctx.failed.end();) {
        if (sched.back() > *test) {
          ++test;
          continue;
        }

        sched.push_back(*test);
        std::vector<bool> results = ctx.run_schedule(sched);

        if (results.back()) {
          ctx.graph->add_edge(*test, sched[sched.size() - 2]);

          if (sched.size() <= rank)
            new_passing.insert(sched);

          test = ctx.failed.erase(test);
        } else {
          ++test;
        }

        sched.pop_back();
      }
    }

    passing.swap(new_passing);
  }
}

schedule MEMFAST::merge_schedules(const schedule &s1, const schedule &s2) {
  schedule result;
  result.reserve(s1.size() + s2.size() + 1);
  uint32_t i = 0, j = 0;

  while (i < s1.size() && j < s2.size()) {
    if (s1[i] < s2[j]) {
      result.push_back(s1[i++]);
    } else if (s1[i] > s2[j]) {
      result.push_back(s2[j++]);
    } else {
      result.push_back(s1[i++]);
      ++j;
    }
  }

  for (; i < s1.size(); ++i)
    result.push_back(s1[i]);
  for (; j < s2.size(); ++j)
    result.push_back(s2[j]);

  return result;
}

std::unique_ptr<Graph> MEMFAST::run(const std::vector<uint32_t> &tests,
                                    TestSuiteOracle *oracle) {
  Context ctx{tests, oracle};

  if (tests.size() == 0)
    return std::move(ctx.graph);

  for (uint32_t rank = 1; rank < tests.size(); ++rank) {
    append_failed_tests(ctx, rank);

    if (ctx.failed.empty())
      break;
    else if (ctx.failed.find(tests[rank]) == ctx.failed.end())
      continue;

    uint32_t prefix_len = 2;
    while (true) {
      extensive_search(ctx, rank, prefix_len);

      if (ctx.failed.find(tests[rank]) == ctx.failed.end())
        break;
      ++prefix_len;
    }
  }

  ctx.graph->transitive_reduction();

  return std::move(ctx.graph);
}

std::unique_ptr<Algorithm> algorithm_factory(const std::string &algo) {
  if (algo == "pradet")
    return std::unique_ptr<Algorithm>{new PraDet{}};
  else if (algo == "pfast")
    return std::unique_ptr<Algorithm>{new PFAST{}};
  else if (algo == "mem-fast")
    return std::unique_ptr<Algorithm>{new MEMFAST{}};
  std::cerr << algo
            << " is not a valid method to find dependencies between tests."
            << std::endl;
  exit(EXIT_FAILURE);
}
