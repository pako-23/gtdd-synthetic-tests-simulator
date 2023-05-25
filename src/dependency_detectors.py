
from abc import ABC, abstractmethod
from graph import Graph
from test_suite_oracle import TestSuiteOracle

def ex_linear(tests: list[int], test_driver: TestSuiteOracle) -> Graph:
    g = Graph(tests)

    for i in range(len(tests)):
        schedule = tests[:i] + tests[i+1:]
        results = test_driver.run_tests(schedule)

        while not all(results):
            index = next((i for i, res in enumerate(results) if not res), -1)
            g.add_edge(schedule[index], tests[i])
            schedule = schedule[:index] + schedule[index+1:]
            results = test_driver.run_tests(schedule)

    return g
