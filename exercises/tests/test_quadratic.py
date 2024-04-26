import unittest
from quadratic.quadratic_equation import quadratic_equation

class TestQuadraticEquation(unittest.TestCase):
    def test_no_solution(self):
        solution = quadratic_equation(-2, 1, -1)
        num_of_solution, _, _ = solution.getSolution()
        self.assertEqual(num_of_solution, 0)

if __name__ == '__main__':
    unittest.main()