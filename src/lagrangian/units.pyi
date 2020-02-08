from typing import Tuple


class Units:
    @staticmethod
    def get_converter(from_: str, to: str) -> Tuple[float, float]:
        ...
