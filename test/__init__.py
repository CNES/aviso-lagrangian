import os
import re
import unittest


def create_test_suite():
    """
    Create the test suite
    """
    suites = []
    pattern = re.compile(r'tests\.(.*)')
    for root, _, files in os.walk('.'):
        for name in files:
            if 'test_' not in name:
                continue
            path = os.path.join(root, name)
            if path.split('.')[-1] == 'py':
                path = path[2:len(path)-3]
                path = path.replace('/' if os.name == 'posix' else '\\', '.')
                m = pattern.search(path)
                if m is not None:
                    path = m.group(0)
                print('*****', path)
                suites.append(path)
    suites = [unittest.defaultTestLoader.loadTestsFromName(item)
              for item in sorted(suites)]
    testSuite = unittest.TestSuite(suites)
    return testSuite


testSuite = create_test_suite()
text_runner = unittest.TextTestRunner(verbosity=3).run(testSuite)
