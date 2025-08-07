# This file is part of lagrangian library.
#
# lagrangian is free software: you can redistribute it and/or modify
# it under the terms of GNU Lesser General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# lagrangian is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of GNU Lesser General Public License
# along with lagrangian. If not, see <http://www.gnu.org/licenses/>.

# Import version using setuptools_scm
try:
    from ._version import version as __version__
except ImportError:
    # Fallback for development installs
    try:
        from setuptools_scm import get_version
        __version__ = get_version(root='..', relative_to=__file__)
    except ImportError:
        __version__ = "unknown"

from .core import (
    CellProperties,
    CoordinatesType,
    DateTime,
    Field,
    FiniteLyapunovExponents,
    FiniteLyapunovExponentsIntegration,
    Integration,
    IntegrationMode,
    Iterator,
    MapOfFiniteLyapunovExponents,
    MapProperties,
    Path,
    Position,
    Quintuplet,
    Reader,
    RungeKutta,
    Stencil,
    TimeDuration,
    Triplet,
    UnitType,
    axis,
    debug,
    field,
    reader,
    units,
    set_verbose,
    version,
)
