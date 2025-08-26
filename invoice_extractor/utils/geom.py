# utils/geom.py
from typing import List, Tuple

# The box is represented by list of values x0, y0, x1, y1
def box_center_y(box: List[float]) -> float:
    return 0.5 * (box[1] + box[3])


def box_center_x(box: List[float]) -> float:
    return 0.5 * (box[0] + box[2])


def expand_box(box: List[float], pad: float, w: float, h: float) -> List[float]:
    x0, y0, x1, y1 = box
    return [max(0, x0 - pad), max(0, y0 - pad), min(w, x1 + pad), min(h, y1 + pad)]
