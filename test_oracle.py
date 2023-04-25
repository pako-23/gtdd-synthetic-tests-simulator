
from abc import ABC, abstractmethod

class TestOracle(ABC):

    n: int

    def __init__(self, n: int) -> None:
        self.n = n

    @abstractmethod
    def generate_tests(self) -> list[int]:
        pass


class ListTestOracle(TestOracle):

    def generate_tests(self) -> list[int]:
        return list(range(self.n))
