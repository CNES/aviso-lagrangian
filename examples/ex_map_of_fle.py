"""
Map of FSLE (Finite-Size Lyapunov Exponent)
===========================================

This example reproduces, in a compact and executable form, the workflow
documented in the console script guide for ``map_of_fle`` and the
unit test ``tests/test_console_scripts.py``. It shows how to:

- Build the configuration of a velocity time series from a list of NetCDF files
  (using the same template as in ``tests/map.ini``)
- Run the console entry-point programmatically to compute a FSLE map
- Open the resulting NetCDF and visualize the main output (``lambda1``)

Computation domain
------------------

We compute a forward FSLE map on a 0.05° grid over [40, 59.95]°E × [-60,
-40.05]°N, with an advection time of 89 days and a final separation of 0.2°.
Those settings mirror the ones used in the unit tests.
"""
from __future__ import annotations

import os
import pathlib
import sys
import tempfile

import matplotlib.pyplot as plt
import netCDF4

import lagrangian

# %%
# The console script we want to run programmatically
from lagrangian.console_scripts.map_of_fle import main as map_of_fle_main

# %%
# Define the ROOT folder of this example
EXAMPLES_DIRECTORY_PATH = pathlib.Path().absolute()

# %%
# Define the base directory of this example
TESTS_DIRECTORY_PATH = EXAMPLES_DIRECTORY_PATH.parent / 'tests'


# %%
# This example relies on built-in test data provided by the lagrangian library.
# To facilitate this, we use the ``SampleDataHandler`` class, which simplifies
# downloading and accessing the required test data. For this example, we will
# override the default behavior to store the downloaded data in the test data
# folder. This folder is writable and allows the data to be reused across
# multiple gallery examples.
class SampleDataHandler(lagrangian.SampleDataHandler):
    ROOT = TESTS_DIRECTORY_PATH


# %%
# Download the built-in test data if not already present
SampleDataHandler()

# %%
# Prepare the configuration file (reusing ``tests/map.ini`` template)
# The template relies on ``${ROOT}``, which we set to the built-in test data.
#
os.environ['ROOT'] = str(SampleDataHandler.folder())
map_ini_path = TESTS_DIRECTORY_PATH / 'map.ini'

# %%
# Define the output NetCDF file path
out_nc = pathlib.Path(tempfile.gettempdir()) / 'fsle_gallery.nc'

# %%
# Defined the command line arguments to run the console script
sys.argv = [
    'map_of_fle',
    str(map_ini_path),
    str(out_nc),
    '2010-01-01',
    '--advection_time=89',
    '--resolution=0.05',
    '--x_min=40',
    '--x_max=59.95',
    '--y_min=-60',
    '--y_max=-40.05',
    '--final_separation=0.2',
    '--time_direction=forward',
]

# %%
# Run the console script programmatically
map_of_fle_main()

# %%
# Open the NetCDF and visualize lambda1 (1/day)
with netCDF4.Dataset(out_nc) as ds:
    lon = ds['lon'][:]
    lat = ds['lat'][:]
    lam1 = ds['lambda1'][:]  # shape (lon, lat)

# %%
# Plot as an image with geographic extent
fig, ax = plt.subplots(figsize=(6, 4))
img = ax.imshow(
    lam1.T,  # transpose to (lat, lon)
    origin='lower',
    extent=[lon.min(), lon.max(), lat.min(),
            lat.max()],
    cmap='viridis',
    interpolation='nearest',
    aspect='auto',
)
ax.set_title('FSLE (lambda1) [1/day]')
ax.set_xlabel('Longitude [deg]')
ax.set_ylabel('Latitude [deg]')
cbar = fig.colorbar(img, ax=ax, orientation='vertical', shrink=0.9)
cbar.set_label('1/day')
plt.tight_layout()
