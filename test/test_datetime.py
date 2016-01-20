import datetime
import lagrangian
import unittest


class TestDateTime(unittest.TestCase):

    def setUp(self):
        pass

    def tearDown(self):
        pass

    def test(self):
        a = datetime.datetime.now()
        b = lagrangian.DateTime(a)()
        self.assertEqual(a, b)
        b = lagrangian.DateTime(a.isoformat(' '))()
        self.assertEqual(a, b)
        try:
            lagrangian.DateTime(1)
        except TypeError:
            pass
        else:
            self.fail('Did not see TypeError')
        self.assertTrue(isinstance(str(lagrangian.DateTime()), str))

if __name__ == "__main__":
    unittest.main()
