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


def infer_column_boundaries(header_row_tokens: List[Dict[str, Any]], min_gap_px: float = 25.0) -> List[Tuple[float, float]]:
    """
    From header tokens, infer column x-intervals. Gaps between header tokens with width>min_gap define boundaries.
    Returns list of (x_left, x_right) for each column, left→right.
    """
    if not header_row_tokens:
        return []
    # Use token boxes to define candidate centers
    header_row_tokens = sorted(header_row_tokens, key=lambda t: t["box"][0])
    xs = []
    for t in header_row_tokens:
        b = t["box"]
        xs.append((b[0], b[2]))
    # Merge overlapping header token spans to coarse columns
    merged: List[Tuple[float, float]] = []
    for (l, r) in xs:
        if not merged or l - merged[-1][1] > min_gap_px:
            merged.append((l, r))
        else:
            merged[-1] = (merged[-1][0], max(merged[-1][1], r))
    # Expand small columns slightly
    cols = []
    for l, r in merged:
        cols.append((float(l), float(r)))
    return cols


def assign_tokens_to_columns(tokens_in_row: List[Dict[str, Any]], columns: List[Tuple[float, float]]) -> List[List[Dict[str, Any]]]:
    """
    Greedy assignment by x-center to the nearest column span. Returns list per column of tokens (left→right).
    """
    if not columns:
        return [tokens_in_row]  # single catch-all
    col_bins: List[List[Dict[str, Any]]] = [[] for _ in columns]
    for t in tokens_in_row:
        cx = box_center_x(t["box"])
        best = min(range(len(columns)),
                   key=lambda j: _dist_to_span(cx, columns[j]))
        col_bins[best].append(t)
    for j in range(len(col_bins)):
        col_bins[j].sort(key=lambda t: t["box"][0])
    return col_bins


def _dist_to_span(x: float, span: Tuple[float, float]) -> float:
    l, r = span
    if x < l:
        return l - x
    if x > r:
        return x - r
    return 0.0
