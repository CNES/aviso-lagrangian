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
from typing import Optional, Tuple
import abc
import datetime
import numpy

from datetime import datetime


def debug(message: str) -> None:
    ...


def version() -> str:
    ...


def set_verbose(value: bool) -> None:
    ...


class CellProperties:
    def __init__(self) -> None:
        ...

    @staticmethod
    def none() -> 'CellProperties':
        ...


class CoordinatesType:
    CARTESIAN: 'CoordinatesType'
    SPHERICAL_EQUATORIAL: 'CoordinatesType'


class UnitType:
    ANGULAR: 'UnitType'
    METRIC: 'UnitType'


class Field:
    def __init__(self,
                 unit_type: UnitType,
                 coordinate_type: CoordinatesType = CoordinatesType.
                 SPHERICAL_EQUATORIAL) -> None:
        ...

    def compute(self,
                t: datetime.datetime,
                x: float,
                y: float,
                cell: CellProperties = CellProperties.none()
                ) -> Tuple[float, float]:
        ...

    def fetch(self, first: datetime.datetime, last: datetime.datetime) -> None:
        ...

    def unit(self) -> str:
        ...

    @property
    def coordinate_type(self) -> CoordinatesType:
        ...

    @property
    def unit_type(self) -> UnitType:
        ...


class FiniteLyapunovExponents:
    def __init__(self) -> None:
        ...

    @property
    def delta_t(self) -> float:
        ...

    @property
    def final_separation(self) -> float:
        ...

    @property
    def lambda1(self) -> float:
        ...

    @property
    def lambda2(self) -> float:
        ...

    @property
    def theta1(self) -> float:
        ...

    @property
    def theta2(self) -> float:
        ...


class IntegrationMode:
    FSLE: 'IntegrationMode'
    FTLE: 'IntegrationMode'


class Iterator:
    def __iter__(self) -> 'Iterator':
        ...

    def __next__(self) -> datetime.datetime:
        ...


class RungeKutta:
    def __init__(self, size_of_interval: float, field: Field) -> None:
        ...

    def compute(self,
                t: datetime.datetime,
                x: float,
                y: float,
                cell: CellProperties = CellProperties.none()
                ) -> Tuple[float, float]:
        ...


class Position:
    def __init__(self,
                 start_time: Optional[datetime.datetime] = None,
                 spherical_equatorial: Optional[bool] = None) -> None:
        ...

    def compute(self, rk4: RungeKutta, it: Iterator,
                cell: CellProperties) -> bool:
        ...

    def max_distance(self) -> float:
        ...

    def strain_tensor(self) -> Tuple[float, float, float, float]:
        ...

    def xi(self) -> numpy.ndarray:
        ...

    def yi(self) -> numpy.ndarray:
        ...

    @property
    def is_completed(self) -> bool:
        ...

    @property
    def missing(self) -> bool:
        ...

    @property
    def time(self) -> datetime.datetime:
        ...


class Triplet(Position):
    def __init__(self,
                 x: float,
                 y: float,
                 delta: float,
                 start_time: datetime.datetime,
                 spherical_equatorial: bool = True) -> None:
        ...


class Quintuplet(Position):
    def __init__(self,
                 x: float,
                 y: float,
                 delta: float,
                 start_time: datetime.datetime,
                 spherical_equatorial: bool = True) -> None:
        ...


class Stencil:
    QUINTUPLET: 'Stencil'
    TRIPLET: 'Stencil'


class Integration:
    def __init__(self,
                 start_time: datetime.datetime,
                 end_time: datetime.datetime,
                 delta_t: datetime.timedelta,
                 field: Field = None) -> None:
        ...

    def compute(self, it: Iterator, x0: float,
                y0: float) -> Tuple[float, float]:
        ...

    def fetch(self, date: datetime.datetime) -> None:
        ...

    def iterator(self) -> Iterator:
        ...


class FiniteLyapunovExponentsIntegration(Integration):
    def __init__(self,
                 start_time: datetime.datetime,
                 end_time: datetime.datetime,
                 delta_t: datetime.timedelta,
                 mode: IntegrationMode,
                 min_sepration: float,
                 delta: float,
                 field: Field = None) -> None:
        ...

    def compute(self, it: Iterator, position: Position,
                cell: CellProperties) -> bool:
        ...

    def exponents(self, position: Position,
                  fsle: FiniteLyapunovExponents) -> bool:
        ...

    def separation(self, position: Position) -> bool:
        ...

    def set_initial_point(self,
                          x: float,
                          y: float,
                          stencil: Stencil,
                          spherical_equatorial: bool = True) -> Position:
        ...

    @property
    def mode(self) -> IntegrationMode:
        ...


class MapProperties:
    def __init__(self, nx: int, ny: int, x_min: float, y_min: float,
                 step: float) -> None:
        ...

    def x_axis(self) -> numpy.ndarray:
        ...

    def y_axis(self) -> numpy.ndarray:
        ...

    @property
    def nx(self) -> int:
        ...

    @property
    def ny(self) -> int:
        ...

    @property
    def step(self) -> float:
        ...

    @property
    def x_min(self) -> float:
        ...

    @property
    def y_min(self) -> float:
        ...


class Reader(abc.ABC):
    @abc.abstractmethod
    def interpolate(self,
                    x: float,
                    y: float,
                    fill_value: float = 0,
                    cell: CellProperties = CellProperties.none()) -> float:
        ...

    @abc.abstractmethod
    def open(self, path: str) -> None:
        ...

    @abc.abstractmethod
    def load(self, name: str, unit: str = "") -> str:
        ...

    @abc.abstractmethod
    def date(self, name: str) -> datetime.datetime:
        ...


class MapOfFiniteLyapunovExponents:
    def __init__(self,
                 map_properties: MapProperties,
                 fle: FiniteLyapunovExponentsIntegration,
                 stencil: Stencil = Stencil.TRIPLET,
                 reader: Reader = None) -> None:
        ...

    def compute(self, num_threads: int = 0) -> None:
        ...

    def map_of_delta_t(self, fill_value: float = numpy.nan) -> numpy.ndarray:
        ...

    def map_of_final_separation(self, fill_value: float = numpy.nan
                                ) -> numpy.ndarray:
        ...

    def map_of_lambda1(self, fill_value: float = numpy.nan) -> numpy.ndarray:
        ...

    def map_of_lambda2(self, fill_value: float = numpy.nan) -> numpy.ndarray:
        ...

    def map_of_theta1(self, fill_value: float = numpy.nan) -> numpy.ndarray:
        ...

    def map_of_theta2(self, fill_value: float = numpy.nan) -> numpy.ndarray:
        ...


class Path(Integration):
    def __init__(self, start_time: datetime.datetime,
                 end_time: datetime.datetime, delta_t: datetime.timedelta,
                 field: Field) -> None:
        ...
