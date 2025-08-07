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
from . import CellProperties, Reader


class Type:
    NETCDF: 'Type'


class NetCDF(Reader):
    def __init__(self) -> None:
        ...

    def interpolate(self,
                    x: float,
                    y: float,
                    fill_value: float = 0,
                    cell: CellProperties = CellProperties.none()) -> float:
        ...

    def open(self, path: str) -> None:
        ...

    def load(self, name: str, unit: str = "") -> str:
        ...

    def date(self, name: str) -> datetime.datetime:
        ...
