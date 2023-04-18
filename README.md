Lagrangian analysis
===================

Introduction
------------

Finite Size Lyapunov Exponent (FSLE) is a local lagrangian diagnostics that is
widely used for the study of transport and mixing processes of oceanographic
tracers (Sea surface temperature, Ocean color ...). Its computation is derived
from the definition of Finite-Time Lyapunov Exponent that allows the
identification of  Lagrangian Coherent Structures.

The code used to compute LE, was developed in collaboration between LOCEAN (F.
d'Ovidio) and CLS, is available under GNU General Public License

Given a time series of meso-scale velocity field, this code computes
corresponding sub-mesoscale maps of backward or forward FLSE or FTLE. It also
compute other diagnostics such as orientation of Finite-Time Lyapunov Vectors.

License
-------

Lagrangian is distributed under the GNU Lesser General Public License. See
COPYING for details.

Documentation
-------------

For full documentation visit the [Documentation
Page](http://lagrangian.readthedocs.io/en/latest/index.html).

Conda
-----

Obtener submodulos:

```{bash}
git submodule update --init --recursive
```

Instalar:

```{bash}
pip install .
```

```{bash}
export UDUNITS2_XML_PATH=/home/atorres/miniconda3/envs/RTD/share/udunits/udunits2.xml
```

```{bash}
time -v map_of_fle sds.ini fsle1.nc "2023-02-08" --advection_time 20 --resolution=0.08333588 --final_separation 0.2 --x_min -180 --x_max 180 --y_min -60 --y_max 60 --verbose --time_direction forward
```

Build package
-------------

```{bash}
conda install -c conda-forge boost udunits2 netcdf4
```

Docker
------

1. Construir imágen

   ```{bash}
   docker build -t lagrangian .
   ```

2. Ejecutar contenedor

   ```{bash}
   docker run --user=root -it -v /home/SHARED/SATLINK/fsle:/home/mambauser/data lagrangian bash
   ```

3. Correr script del cálculo del FSLE

   ```{bash}
   python map_of_fle.py ./data/uv ./data/fsle "2023-02-28 12:00" --advection_time 20 --final_separation 0.2 --x_min -180 --nx 4320 --y_min -60 --ny 1441 --resolution 1/12 --time_direction backward
   ```
