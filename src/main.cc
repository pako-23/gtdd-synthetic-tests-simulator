#include "algorithms.h"
#include "test-suite-oracle.h"
#include "test-suite.h"
#include <cstdlib>
#include <fstream>
#include <getopt.h>
#include <limits>
#include <string.h>

int generate_command(int, char *[]);
int deps_command(int, char *[]);
int help_command(int, char *[]);
void print_root_help(const char *);
void print_deps_help(const char *);
void print_generate_help(const char *);

int main(int argc, char *argv[]) {
  if (argc < 2) {
    print_root_help(argv[0]);
    return EXIT_FAILURE;
  }

  ++optind;

  if (strcmp(argv[1], "generate") == 0)
    return generate_command(argc, argv);
  else if (strcmp(argv[1], "deps") == 0)
    return deps_command(argc, argv);
  else if (strcmp(argv[1], "help") == 0)
    return help_command(argc, argv);

  print_root_help(argv[0]);
  return EXIT_FAILURE;
}

int generate_command(int argc, char *argv[]) {
  static struct option options[] = {{"generator", required_argument, 0, 'g'},
                                    {"max-out", required_argument, 0, '0'},
                                    {"min-out", required_argument, 0, '1'},
                                    {"output", required_argument, 0, 'o'},
                                    {"probability", required_argument, 0, 'p'},
                                    {"tests", required_argument, 0, 't'},
                                    {"help", no_argument, 0, 'h'},
                                    {0, 0, 0, 0}};
  int opt{0}, long_index{0};
  uint32_t n{0};
  GraphGeneratorParams generator_params = {
      .probability = std::numeric_limits<double>::infinity(),
      .min_out = 3,
      .max_out = 3,
  };
  char *out_file{nullptr};

  while ((opt = getopt_long(argc, argv, "g:t:p:o:h", options, &long_index)) !=
         -1) {
    switch (opt) {
    case 'g':
      generator_params.generator_type = optarg;
      break;
    case 't':
      n = strtol(optarg, 0, 10);
      break;
    case 'o':
      out_file = optarg;
      break;
    case 'p':
      generator_params.probability = atof(optarg);
      break;
    case '0':
      generator_params.max_out = strtol(optarg, 0, 10);
      break;
    case '1':
      generator_params.min_out = strtol(optarg, 0, 10);
      break;
    case 'h':
      print_generate_help(argv[0]);
      return EXIT_SUCCESS;
    default:
      print_generate_help(argv[0]);
      return EXIT_FAILURE;
    }
  }

  if (!n || generator_params.generator_type.empty()) {
    print_generate_help(argv[0]);
    return EXIT_FAILURE;
  }

  ListTestSuite suite{n};
  std::vector<uint32_t> tests = suite.generate_tests();
  std::unique_ptr<DirectDependenciesOracle> oracle{
      new DirectDependenciesOracle{tests, generator_params}};

  if (!out_file) {
    std::cout << oracle->get_graph();
    return EXIT_SUCCESS;
  }

  std::ofstream out{out_file};
  if (!out) {
    std::cerr << "Unable to open output file \"" << out_file << '"'
              << std::endl;
    return EXIT_FAILURE;
  }
  out << oracle->get_graph();

  return EXIT_SUCCESS;
}

int deps_command(int argc, char *argv[]) {
  static struct option options[] = {{"input", required_argument, 0, 'i'},
                                    {"algorithm", required_argument, 0, 'a'},
                                    {"output", required_argument, 0, 'o'},
                                    {"metrics", required_argument, 0, 'm'},
                                    {"help", no_argument, 0, 'h'},
                                    {0, 0, 0, 0}};
  int opt{0}, long_index{0};
  char *out_file{nullptr};
  char *metric_file{nullptr};
  char *input_file{nullptr};
  char *algorithm{nullptr};

  while ((opt = getopt_long(argc, argv, "i:a:o:m:h", options, &long_index)) !=
         -1) {
    switch (opt) {
    case 'i':
      input_file = optarg;
      break;
    case 'a':
      algorithm = optarg;
      break;
    case 'o':
      out_file = optarg;
      break;
    case 'm':
      metric_file = optarg;
      break;
    case 'h':
      print_deps_help(argv[0]);
      return EXIT_SUCCESS;
    default:
      print_deps_help(argv[0]);
      return EXIT_FAILURE;
    }
  }

  if (!input_file || !algorithm) {
    print_deps_help(argv[0]);

    return EXIT_FAILURE;
  }

  Graph g;
  std::ifstream is{input_file};
  if (!is) {
    std::cerr << "Could not open input file \"" << input_file << '"'
              << std::endl;
    return EXIT_FAILURE;
  }

  is >> g;

  std::unique_ptr<TestSuiteOracle> oracle{new DirectDependenciesOracle{g}};
  std::vector<uint32_t> tests{oracle->tests()};
  Metrics m{tests.size()};

  m.optimal_schedule_metrics(g.get_schedules());
  std::unique_ptr<Algorithm> algo{algorithm_factory(algorithm)};
  std::unique_ptr<Result> result{algo->run(tests, oracle.get(), m)};
  result->output(out_file);
  if (metric_file)
    m.record(metric_file);

  return EXIT_SUCCESS;
}

int help_command(int argc, char *argv[]) {

  if (argc != 3) {
    print_root_help(argv[0]);
    return EXIT_FAILURE;
  }

  if (strcmp(argv[2], "generate") == 0)
    print_generate_help(argv[0]);
  else if (strcmp(argv[2], "deps") == 0)
    print_deps_help(argv[0]);
  else {
    print_root_help(argv[0]);
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}

void print_root_help(const char *prog_name) {
  std::cout << "A tool for simulating test suite dependency detection"
            << std::endl
            << "algorithms that generates some synthetic test" << std::endl
            << "dependencies." << std::endl
            << std::endl
            << "Usage: " << std::endl
            << "  " << prog_name << " [command]" << std::endl
            << std::endl
            << "Available commands:" << std::endl
            << "  generate  Generates a synthetic test suite." << std::endl
            << "  deps      Finds dependencies between tests of a test suite."
            << std::endl
            << std::endl
            << "Use \"" << prog_name << " help [command]\" for more information"
            << " about a command." << std::endl;
}

void print_deps_help(const char *prog_name) {
  std::cout
      << "Find the dependencies between the tests into a test suite."
      << std::endl
      << std::endl
      << "Usage: " << std::endl
      << "  " << prog_name << " deps [flags]" << std::endl
      << std::endl
      << "Flags:" << std::endl
      << "  -i, --input file      The file containing the synthetic test suite "
         "(Required)"
      << std::endl
      << "  -a, --algorithm algo  The algorithm to use to find dependencies. "
         "(Required)"
      << std::endl
      << "                        The possible values are: pradet, pfast, "
         "mem-fast."
      << std::endl
      << "  -o, --output file     The file to store dependency detection "
         "results. (Default: stdout)"
      << std::endl
      << "  -m, --metrics file    The file to store metrics about the run."
      << std::endl
      << "  -h, --help            Display this help page." << std::endl
      << std::endl;
}

void print_generate_help(const char *prog_name) {
  std::cout << "Generate a synthetic test suite using some heuristic."
            << std::endl
            << std::endl
            << "Usage: " << std::endl
            << "  " << prog_name << " generate [flags]" << std::endl
            << std::endl
            << "Flags:" << std::endl
            << "  -g, --generator gen  The strategy to generate edges in the "
               "dependency graph. (Required)"
            << std::endl
            << "                       The possible values are: erdos-renyi, "
               "barabasi-albert, out-degree."
            << std::endl
            << "  --max-out n          The maximum outgoing degree in the "
               "out-degree generator. (Default: 3)"
            << std::endl
            << "  --min-out n          The minimum outgoing degree in the "
               "out-degree generator. (Default: 3)"
            << std::endl
            << "  -p, --probability n  The probability to use in Erdos-Renyi "
               "edge generation. (Default: log(nodes)/nodes)"
            << std::endl
            << "  -o, --output file    The file to store the synthetic test "
               "suite. (Default: stdout)"
            << std::endl
            << "  -t, --tests n        The number of tests to generate into "
               "the test suite. (Required)"
            << std::endl
            << "  -h, --help           Display this help page." << std::endl
            << std::endl;
}
