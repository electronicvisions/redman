import unittest
import warnings

class TestPyRedmanImport(unittest.TestCase):
    """
    Test import, this will not work correctly if redman was imported globally
    """

    def test_import(self):
        with warnings.catch_warnings(record=True) as w:
            warnings.filterwarnings('error')
            warnings.filterwarnings('ignore', category=ImportWarning)
            import pyredman


if __name__ == '__main__':
    unittest.main()
