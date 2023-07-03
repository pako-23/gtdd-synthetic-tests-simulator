#include "algorithms.h"
#include "test-suite-oracle.h"
#include "test-suite.h"
#include "utils.h"
#include <fstream>
#include <ctime>
#include <limits>

const char RESULTS_PATH[] = "./results";

int main(int argc, const char *argv[])
{
    uint32_t n;
    std::string algorithm;

    GraphGeneratorParams oracle_params = {
        .probability = std::numeric_limits<double>::infinity(),
        .min_out = 3,
        .max_out = 3,
    };

    ArgParser args{argv[0], "Program to run tests on synthetic graphs"};
    args.add_parameter(new ValuedParameter<uint32_t>("tests", "The number of tests", n, "t", true));
    args.add_parameter(new ValuedParameter<std::string>("algorithm",
        (
            "The algorithm to run. The possible values are: " +
            join(", ", "pradet", "ex-linear", "big-table")
        ).c_str(), algorithm, "a", true));
    args.add_parameter(new ValuedParameter<std::string>("graph-generator",
        (
            "The approach used to generate the graph of dependencies. "
            "The possible values are: " +
            join(", ", "erdos-renyi", "barabasi-albert", "out-degree")
        ).c_str(), oracle_params.generator_type, "g", true));
    args.add_parameter(new ValuedParameter<double>("probability",
        "Erdos-Renyi probability", oracle_params.probability, "p", false));
    args.add_parameter(new ValuedParameter<uint32_t>("min-out",
        "Min out degree", oracle_params.min_out, nullptr, false));
    args.add_parameter(new ValuedParameter<uint32_t>("max-out",
        "Max out degree", oracle_params.max_out, nullptr, false));
    args.parse(argc, argv);

    std::string gen = oracle_params.generator_type;
    if (gen == "out-degree") gen = join("-", gen, oracle_params.min_out, oracle_params.max_out);

    create_dir(RESULTS_PATH);
    create_dir(join("/", RESULTS_PATH, algorithm).c_str());
    create_dir(join("/", RESULTS_PATH, algorithm, gen).c_str());
    std::string prefix = join("/", RESULTS_PATH, algorithm, gen, n);
    create_dir(prefix.c_str());

    std::string stats_file = join("/", prefix, "stats.txt");
    std::ofstream log{stats_file, std::ios_base::app | std::ios_base::out};


    time_t t = std::time(nullptr);
    tm *tm = std::localtime(&t);

    ListTestSuite suite{n};
    std::vector<uint32_t> tests = suite.generate_tests();


    DirectDependenciesOracle *oracle =
        new DirectDependenciesOracle{tests, oracle_params};

    std::ofstream graph_out{join("/", prefix, graphviz_filename("truth", tm))};
    graph_out << oracle->get_generator()->get_graph();
    graph_out.close();

    Algorithm *algo = algorithm_factory(algorithm, join("/", prefix, graphviz_filename("results", tm)));

    Metrics *m = algo->run(tests, oracle);

    log << n << ' ' << m->test_suite_runs << ' ' << m->tests_run << std::endl;

    log.close();
    delete oracle;
    delete m;
    delete algo;

    return EXIT_SUCCESS;
}
