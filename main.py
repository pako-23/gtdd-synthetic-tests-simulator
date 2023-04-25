#!/usr/bin/env python3

from test_oracle import ListTestOracle, TestOracle
from graph import GraphGenerators
from test_driver import TestDriver, DirectDependenciesTestDriver
from dependency_detectors import ex_linear
import os

def main():
    n = 10
    if not os.path.isdir("./results"):
        os.mkdir("./results")
    oracle: TestOracle = ListTestOracle(n)
    tests: list[int] = oracle.generate_tests()
    driver: TestDriver = DirectDependenciesTestDriver(n, GraphGenerators.ErdosRenyi)
    g = ex_linear(tests, driver)
    g.transitive_reduction()
    with open('./results/computed_graph.gv', 'w') as fp:
        print(g, file=fp)

if __name__ == "__main__":
    main()
