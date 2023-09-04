#include "algorithms.h"
#include "test-suite-oracle.h"
#include "test-suite.h"
#include "utils.h"
#include <ctime>
#include <fstream>
#include <limits>
#include <memory>
const char RESULTS_PATH[] = "./results";

int main(int argc, const char* argv[])
{
    uint32_t n;
    std::string algorithm;

    GraphGeneratorParams oracle_params = {
        .probability { std::numeric_limits<double>::infinity() },
        .min_out { 3 },
        .max_out { 3 },
    };

    ArgParser args { argv[0], "Program to run tests on synthetic graphs" };
    args.add_parameter(new ValuedParameter<uint32_t>(
        "tests", "The number of tests", n, "t", true));
    args.add_parameter(new ValuedParameter<std::string>(
        "algorithm",
        ("The algorithm to run. The possible values are: " + join(", ", "pradet", "ex-linear", "big-table"))
            .c_str(),
        algorithm,
        "a",
        true));
    args.add_parameter(new ValuedParameter<std::string>(
        "graph-generator",
        ("The approach used to generate the graph of dependencies. "
         "The possible values are: "
            + join(", ", "erdos-renyi", "barabasi-albert", "out-degree"))
            .c_str(),
        oracle_params.generator_type,
        "g",
        true));
    args.add_parameter(new ValuedParameter<double>("probability",
        "Erdos-Renyi probability",
        oracle_params.probability,
        "p",
        false));
    args.add_parameter(new ValuedParameter<uint32_t>(
        "min-out", "Min out degree", oracle_params.min_out, nullptr, false));
    args.add_parameter(new ValuedParameter<uint32_t>(
        "max-out", "Max out degree", oracle_params.max_out, nullptr, false));
    args.parse(argc, argv);

    std::string gen = oracle_params.generator_type;
    if (gen == "out-degree")
        gen = join("-", gen, oracle_params.min_out, oracle_params.max_out);

    create_dir(RESULTS_PATH);
    create_dir(join("/", RESULTS_PATH, algorithm).c_str());

    std::string prefix { join("/", RESULTS_PATH, algorithm, gen) };
    create_dir(prefix.c_str());

    std::string graph_dir { join("/", prefix, n) };
    create_dir(graph_dir.c_str());

    std::string stats_file { join("/", prefix, "stats.csv") };
    bool add_header { !file_exists(stats_file.c_str()) };
    std::ofstream log { stats_file, std::ios_base::app | std::ios_base::out };
    if (add_header)
        log << Metrics::get_header() << std::endl;

    time_t t = std::time(nullptr);
    tm* tm = std::localtime(&t);

    Metrics m { n };
    ListTestSuite suite { n };
    std::vector<uint32_t> tests = suite.generate_tests();

    std::unique_ptr<DirectDependenciesOracle> oracle { new DirectDependenciesOracle { tests, oracle_params } };

    std::ofstream graph_out { join("/", graph_dir, graphviz_filename("truth", tm)) };
    graph_out << oracle->get_generator()->get_graph();
    graph_out.close();

    m.optimal_schedule_metrics(oracle->get_generator()->get_graph().get_schedules());

    std::unique_ptr<Algorithm> algo { algorithm_factory(algorithm, join("/", graph_dir, graphviz_filename("results", tm))) };

    algo->run(tests, oracle.get(), m);

    log << m << std::endl;

    log.close();

    return EXIT_SUCCESS;
}
