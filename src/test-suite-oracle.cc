#include "test-suite-oracle.h"

DirectDependenciesOracle::DirectDependenciesOracle(const std::vector<uint32_t>& nodes,
                                                   const GraphGeneratorParams& params)
    : graph_generator{nullptr}
{
    if (params.generator_type == "erdos-renyi")
        graph_generator = new ErdosRenyiGenerator{nodes, params.probability};
    else if (params.generator_type == "barabasi-albert")
        graph_generator = new BarabasiAlbertGenerator{nodes};
    else if (params.generator_type == "out-degree")
        graph_generator = new OutDegreeGenerator{nodes, params.min_out, params.max_out};
    else {
        std::cerr << params.generator_type << " is not a valid method to generate graphs.";
        exit(EXIT_FAILURE);
    }
    graph_generator->generate_edges();
}

DirectDependenciesOracle::~DirectDependenciesOracle()
{
    delete graph_generator;
}

std::vector<bool> DirectDependenciesOracle::run_tests(const std::vector<uint32_t>& tests)
{
    std::vector<bool> results(tests.size());

    for (auto it1 = tests.begin(); it1 != tests.end(); ++it1) {
        std::unordered_set<uint32_t> deps =
            graph_generator->get_graph().get_dependencies(*it1);

        for (const uint32_t dep : deps) {
            bool satisfied{false};
            for (auto it2 = tests.begin(); it2 != it1; ++it2)
                if (*it2 == dep) satisfied = true;

            if (!satisfied) return results;
        }

        results[it1 - tests.begin()] = true;
    }

    return results;
}
