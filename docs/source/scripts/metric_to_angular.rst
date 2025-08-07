metric_to_angular
=================

When calculating FSLE or FTLE grids, if the velocity fields are expressed in
``m/s``, the program ``map_of_fle.py`` must make a change of reference in order
to calculate how many degrees the particle has been moved.

To avoid this costly conversion in terms of computation time, run this program
to change the unit of grids at the input from ``m/s`` to ``degrees/s``.

Type ``metric_to_angular.py --help`` to see the available options.
