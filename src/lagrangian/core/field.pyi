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
import datetime
from . import Field, UnitType, CoordinatesType
from . import reader


class Python(Field):
    def __init__(self,
                 parent: object,
                 unit_type: UnitType = UnitType.METRIC,
                 coordinates_type: CoordinatesType = CoordinatesType.CARTESIAN
                 ) -> None:
        ...


class TimeSerie(Field):
    def __init__(self,
                 configuration_file: str,
                 unit_type: UnitType = UnitType.METRIC,
                 coordinates_type: CoordinatesType = CoordinatesType.
                 SPHERICAL_EQUATORIAL,
                 reader_type: reader.Type = reader.Type.NETCDF) -> None:
        ...

    def end_time(self) -> datetime.datetime:
        ...

    def start_time(self) -> datetime.datetime:
        ...


class Vonkarman(Field):
    def __init__(self) -> None:
        ...
