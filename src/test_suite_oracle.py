
from abc import ABC, abstractmethod
import graph

class TestSuiteOracle:

    @abstractmethod
    def run_tests(self, tests: list[int]) -> list[bool]:
        pass


class DirectDependenciesOracle(TestSuiteOracle):

    graph_generator: graph.GraphGenerator

    def __init__(self, n: int, graph_generator: graph.GraphGenerators) -> None:
        if graph_generator == graph.GraphGenerators.ErdosRenyi:
            self.graph_generator = graph.ErdosRenyiGenerator(n)
        elif graph_generator == graph.GraphGenerators.BarabasiAlbert:
            self.graph_generator = graph.BarabasiAlbertGenerator(n)
        else:
            raise ValueError("Not a valid graph generation algorithm")
        self.graph_generator.generate_edges()
        self.graph_generator.G.transitive_reduction()
        with open('./results/real_graph.gv', 'w') as fp:
            print(str(self.graph_generator.G), file=fp)

    def run_tests(self, tests: list[int]) -> list[bool]:
        results = []

        for i in range(len(tests)):
            deps = self.graph_generator.G.get_dependencies(tests[i])
            results.append(deps.intersection(set(tests[:i])) == deps)

        return results
