[![Build Status](https://drone.io/bitbucket.org/cnes_aviso/lagrangian/status.png)](https://drone.io/bitbucket.org/cnes_aviso/lagrangian/latest)

Welcome
=======

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

Install with Ubuntu package
===========================

Now you can install the library from a Debian Package, if you are using Ubuntu
12.04 or 16.04. Under Ubuntu 12.04, the package provides a library for Python2
and for Ubuntu 16.04 a library for Python3. To install,

1. download the wanted archive:

        wget https://bitbucket.org/cnes_aviso/lagrangian/downloads/ubuntu_16_04.zip

2. unzip it:

        unzip ubuntu_16_04.zip

3. then install (you need root privilege to launch this command):

        sudo apt-get install ubuntu_16_04/python3-lagrangian_2.1.0_amd64.deb


If you got some dependency problems, the you need to execute this last command to fix it:

    sudo apt install -f

First steps with the software
=============================
[To know on how to build the library from
sources](https://bitbucket.org/cnes_aviso/lagrangian/wiki/Install) and [how to
use the software](https://bitbucket.org/cnes_aviso/lagrangian/wiki/Usage)

Contents
========
[For a complete description on Lagrangian
analysis](https://bitbucket.org/cnes_aviso/lagrangian/wiki/Contents)