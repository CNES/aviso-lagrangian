#!/bin/bash
set -e
CXXFLAGS="-I${PREFIX}/include" ${PREFIX}/bin/python setup.py build
${PREFIX}/bin/python setup.py install
