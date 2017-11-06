#!/bin/bash
set -e
$PYTHON setup.py setup --netcdf-includes=$CONDA_DEFAULT_ENV/include --netcdf-libraries=$CONDA_DEFAULT_ENV/lib --boost-mt=0
$PYTHON setup.py config
$PYTHON setup.py build
$PYTHON setup.py install
