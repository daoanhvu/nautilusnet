# scripts/rowcol_infer.py
from typing import List, Dict, Any, Tuple, Optional
import numpy as np
from sklearn.cluster import AgglomerativeClustering
from utils.geom import box_center_x, box_center_y


def cluster_rows(tokens: List[Dict[str, Any]], distance_px: float = 15.0) -> List[List[int]]:
    if not tokens:
        return []

    if len(tokens) == 1:
        return [0]

    ys = np.array([[box_center_y(t["box"])] for t in tokens])
    clutering = AgglomerativeClustering(
        n_clusters=None, distance_threshold=distance_px, linkage="single"
    ).fit(ys)