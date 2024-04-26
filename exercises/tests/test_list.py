import unittest
import list_exercises

class TestList(unittest.TestCase):
    def test_median(self):
        data = [7, 4, 5, 3, 4]
        actual_median = list_exercises.calculate_median(data)
        self.assertEqual(actual_median, 4)

if __name__ == '__main__':
    unittest.main()