#include "test-suite-oracle.h"
#include <fstream>

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

DirectDependenciesOracle::DirectDependenciesOracle(const Graph &g) : graph{g} {}

std::vector<bool>
DirectDependenciesOracle::run_tests(const std::vector<uint32_t> &tests) {
  std::vector<bool> results(tests.size());

  for (auto it1 = tests.begin(); it1 != tests.end(); ++it1) {
    std::unordered_set<uint32_t> deps = graph.get_dependencies(*it1);

    for (const uint32_t dep : deps) {
      bool satisfied{false};
      for (auto it2 = tests.begin(); it2 != it1; ++it2)
        if (*it2 == dep)
          satisfied = true;

      if (!satisfied)
        return results;
    }

    results[it1 - tests.begin()] = true;
  }

  return results;
}

std::vector<uint32_t> DirectDependenciesOracle::tests() const {
  std::vector<uint32_t> v;
  for (const auto &it : graph)
    v.push_back(it.first);
  return v;
}

std::unique_ptr<TestSuiteOracle> from_file(const char *fname) {
  Graph g;
  std::ifstream is{fname};
  is >> g;
  return std::unique_ptr<TestSuiteOracle>{new DirectDependenciesOracle{g}};
}
