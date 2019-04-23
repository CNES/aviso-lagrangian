#!/bin/bash
set -e
${PREFIX}/bin/python setup.py setup --netcdf-includes=${PREFIX}/include --netcdf-libraries=${PREFIX}/lib --boost-mt=0
${PREFIX}/bin/python setup.py config
${PREFIX}/bin/python setup.py build
${PREFIX}/bin/python setup.py install
