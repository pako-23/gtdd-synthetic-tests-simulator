
from abc import ABC, abstractmethod
import enum
import random
import copy

class GraphGenerators(enum.Enum):
    ErdosRenyi = "ErdosRenyi"
    BarabasiAlbert = "BarabasiAlbert"

class Graph:

    graph: dict[int, set[int]]

    def __init__(self, nodes: list[int]) -> None:
        self.graph = dict([(i, set()) for i in nodes])

    def add_edge(self, u: int, v: int) -> None:
        assert u in self.graph
        assert v in self.graph
        self.graph[u].add(v)

    def in_degree(self, u) -> int:
        return sum([1 for edges in self.graph.values() if u in edges])

    def get_dependencies(self, u: int) -> list[int]:
        dep = set()

        q = [u]
        visited = {u}

        while (len(q) != 0):
            v = q.pop()
            for i in self.graph[v]:
                if i not in visited:
                    q.append(i)
                    dep.add(i)
            visited.add(v)

        return dep

    def transitive_reduction(self) -> None:
        for node, edges in self.graph.items():
            min_edges = copy.deepcopy(edges)
            for v in edges:
                deps = self.get_dependencies(v)
                for u in edges:
                    if u in deps and u in min_edges:
                        min_edges.remove(u)
            self.graph[node] = min_edges

    def __len__(self) -> int:
        return len(self.graph)

    def __str__(self):
        lines = []
        lines.append("digraph G {")

        for node, edges in self.graph.items():
            for target in edges:
                lines.append(f"    {node} -> {target};")

        lines.append("}")
        return "\n".join(lines)

class GraphGenerator(ABC):
    _graph: Graph

    def __init__(self, nodes: int) -> None:
        self._graph = Graph(range(nodes))

    @abstractmethod
    def generate_edges(self) -> None:
        pass

    @property
    def G(self):
        return self._graph


class ErdosRenyiGenerator(GraphGenerator):
    p: float = 0.5

    def generate_edges(self) -> None:
        for i in range(len(self.G)):
            for j in range(i+1, len(self.G)):
                if random.uniform(0, 1) < self.p:
                    self.G.add_edge(j, i)

class BarabasiAlbertGenerator(GraphGenerator):

    def generate_edges(self) -> None:
        assert len(self.G) >= 2
        self.G.add_edge(2, 1)
        for v in range(3, len(self.G)):
            s = sum([self.G.in_degree(j) for j in range(v)])
            for i in range(v):
                if random.uniform(0, 1) < self.G.in_degree(i)/s:
                    self.G.add_edge(v, i)
