#include "test-suite-oracle.h"
#include <cstdint>
#include <memory>

DirectDependenciesOracle::DirectDependenciesOracle(
    const std::vector<uint32_t> &nodes, const GraphGeneratorParams &params)
    : graph{nodes} {

  std::unique_ptr<GraphGenerator> graph_generator;
  if (params.generator_type == "erdos-renyi")
    graph_generator.reset(new ErdosRenyiGenerator{graph, params.probability});
  else if (params.generator_type == "barabasi-albert")
    graph_generator.reset(new BarabasiAlbertGenerator{graph});
  else if (params.generator_type == "out-degree")
    graph_generator.reset(
        new OutDegreeGenerator{graph, params.min_out, params.max_out});
  else {
    std::cerr << '"' << params.generator_type << '"'
              << " is not a valid method to generate graphs." << std::endl;
    exit(EXIT_FAILURE);
  }
  graph_generator->generate_edges();
}

DirectDependenciesOracle::DirectDependenciesOracle(const Graph &g)
    : graph{g}, test_suite_runs{0}, test_runs{0} {}

std::vector<bool>
DirectDependenciesOracle::run_tests(const std::vector<uint32_t> &tests) {
  std::vector<bool> results(tests.size(), false);

  ++test_suite_runs;

  for (uint32_t i = 0; i < tests.size(); ++i) {
    std::unordered_set<uint32_t> deps = graph.get_dependencies(tests[i]);

    for (const uint32_t dep : deps) {
      bool satisfied{false};
      for (uint32_t j = 0; j != i; ++j)
        if (tests[j] == dep)
          satisfied = true;

      if (!satisfied) {
        test_runs += i + 1;
        return results;
      }
    }

    results[i] = true;
  }

  test_runs += tests.size();

  return results;
}

std::vector<uint32_t> DirectDependenciesOracle::tests(void) const {
  std::vector<uint32_t> v;
  for (const auto &it : graph)
    v.push_back(it.first);
  return v;
}
