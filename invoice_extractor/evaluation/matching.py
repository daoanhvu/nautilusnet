"""Optimal alignment of predicted line items to ground-truth line items.

Row order is not reliable (OCR reading order, merged/split rows), so items are
matched by content rather than position, using Hungarian assignment over a
similarity matrix. A pair below MATCH_THRESHOLD is rejected, leaving the rows
to be counted as a false positive and a false negative respectively.
"""
from __future__ import annotations

from typing import Any, Dict, List, Sequence, Tuple

import numpy as np
from scipy.optimize import linear_sum_assignment

from evaluation.normalize import (
    parse_money, parse_qty, text_similarity,
)

MATCH_THRESHOLD = 0.45

# Description dominates, but the numbers help when OCR mangles the text.
_W_DESC, _W_TOTAL, _W_QTY, _W_PRICE = 0.60, 0.20, 0.10, 0.10


def _num_agreement(a: Any, b: Any, parser) -> float:
    pa, pb = parser(a), parser(b)
    if pa is None and pb is None:
        return 0.5           # both absent: weak evidence, neither for nor against
    if pa is None or pb is None:
        return 0.0
    if abs(pa - pb) <= 0.01:
        return 1.0
    denom = max(abs(pa), abs(pb), 1e-9)
    return max(0.0, 1.0 - abs(pa - pb) / denom)


def item_similarity(pred: Dict[str, Any], gold: Dict[str, Any]) -> float:
    return (
        _W_DESC * text_similarity(pred.get("description"), gold.get("description"))
        + _W_TOTAL * _num_agreement(pred.get("line_total"), gold.get("line_total"), parse_money)
        + _W_QTY * _num_agreement(pred.get("quantity"), gold.get("quantity"), parse_qty)
        + _W_PRICE * _num_agreement(pred.get("unit_price"), gold.get("unit_price"), parse_money)
    )


def match_items(
    pred_items: Sequence[Dict[str, Any]],
    gold_items: Sequence[Dict[str, Any]],
    threshold: float = MATCH_THRESHOLD,
) -> Tuple[List[Tuple[int, int, float]], List[int], List[int]]:
    """Return (matched_triples, unmatched_pred_idx, unmatched_gold_idx).

    Each triple is (pred_index, gold_index, similarity).
    """
    if not pred_items or not gold_items:
        return [], list(range(len(pred_items))), list(range(len(gold_items)))

    sim = np.zeros((len(pred_items), len(gold_items)), dtype=float)
    for i, p in enumerate(pred_items):
        for j, g in enumerate(gold_items):
            sim[i, j] = item_similarity(p, g)

    rows, cols = linear_sum_assignment(-sim)     # maximise similarity

    matched: List[Tuple[int, int, float]] = []
    used_pred, used_gold = set(), set()
    for i, j in zip(rows, cols):
        if sim[i, j] >= threshold:
            matched.append((int(i), int(j), float(sim[i, j])))
            used_pred.add(int(i))
            used_gold.add(int(j))

    unmatched_pred = [i for i in range(len(pred_items)) if i not in used_pred]
    unmatched_gold = [j for j in range(len(gold_items)) if j not in used_gold]
    return matched, unmatched_pred, unmatched_gold
