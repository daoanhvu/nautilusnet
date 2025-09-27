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
    labels = clutering.labels_
    rows: List[List[int]] = []

    for lab in np.unique(labels):
        idxs = np.where(labels == lab)[0].tolist()
        # Sort left -> right within row
        idxs.sort(key=lambda i: tokens[i]["box"][0])
        rows.append(idxs)

    # Sort rows top -> bottom by average y
    rows.sort(key=lambda idxs: float(
        np.mean([box_center_y(tokens[i]["box"]) for i in idxs])))
    return rows
