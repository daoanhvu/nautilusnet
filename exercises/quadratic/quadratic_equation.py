import math
import json

class QuadraticSolution:
    def __init__(self, s, _x1, _x2) -> None:
        self.num_of_solution = s
        self.x1 = _x1
        self.x2 = _x2

    def getSolution(self):
        return self.num_of_solution, self.x1, self.x2
    
    def getSolutionJson(self):
        str = "Have two solutions in R"
        if (self.num_of_solution == -1):
            str = "Have solutions for any x in R"
        else:
            if (self.num_of_solution == 1):
                str = "Have only one solution in R"
            else:
                if (self.num_of_solution == 0):
                    str = "Have no solution in R"
        return {
            "solution": str,
            "x1": self.x1,
            "x2": self.x2
        }

def quadratic_equation(a, b, c) -> QuadraticSolution:
    if (a != 0):
        d = b * b - 4 * a * c
        if (d < 0):
            solution = QuadraticSolution(0, 0, 0)
            return solution
        else:
            if (d > 0):
                sqrt_d = math.sqrt(d)
                twoA = 2 * a
                x1 = -b + sqrt_d / twoA
                x2 = -b - sqrt_d / twoA
                solution = QuadraticSolution(2, x1, x2)
                return solution
            
            x = -b / (2 * a)
            solution = QuadraticSolution(1, x, x)
    else:
        if (b != 0):
            x = -c / b
            solution = QuadraticSolution(a, x, x)
        else:
            if (c != 0):
                solution = QuadraticSolution(0, 0, 0)
            else:
                solution = QuadraticSolution(-1, 0, 0)
    
if __name__ == '__main__':
    solution = quadratic_equation(4.6, 2, -1)
    solution_str = json.dumps(solution.getSolutionJson())
    print(solution_str)