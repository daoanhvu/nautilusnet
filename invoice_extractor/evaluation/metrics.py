"""Scoring.

Null handling is the part that matters: gold-null/pred-null counts as correct,
gold-null/pred-value counts as a hallucination, gold-value/pred-null counts as
a miss. All three are scored, so a model cannot inflate its number by
declining to answer.
"""
from __future__ import annotations

from dataclasses import dataclass, field
from typing import Any, Callable, Dict, List, Optional, Sequence

from evaluation.matching import match_items
from evaluation.normalize import (
    int_equal, money_equal, qty_equal, text_equal, unit_equal,
)

# Scalar fields, mapped onto the project's stated goals.
#   (report name, dotted path, comparator)
SCALAR_FIELDS: List[tuple] = [
    ("seller.name",      "seller.name",      text_equal),
    ("invoice.number",   "number",           text_equal),
    ("item_count",       "item_count",       int_equal),
    ("totals.subtotal",  "totals.subtotal",  money_equal),
    ("totals.discount",  "totals.discount",  money_equal),
    ("totals.tax_total", "totals.tax_total", money_equal),
    ("totals.total",     "totals.total",     money_equal),
]

ITEM_FIELDS: List[tuple] = [
    ("description", text_equal),
    ("unit",        unit_equal),
    ("quantity",    qty_equal),
    ("unit_price",  money_equal),
    ("line_total",  money_equal),
]


def _dig(obj: Dict[str, Any], path: str) -> Any:
    cur: Any = obj
    for part in path.split("."):
        if not isinstance(cur, dict):
            return None
        cur = cur.get(part)
    return cur


@dataclass
class InvoiceScore:
    name: str
    scalars: Dict[str, bool] = field(default_factory=dict)
    item_tp: int = 0
    item_fp: int = 0
    item_fn: int = 0
    item_fields: Dict[str, List[bool]] = field(default_factory=dict)
    rows_fully_correct: int = 0
    error_notes: List[str] = field(default_factory=list)

    @property
    def item_precision(self) -> float:
        d = self.item_tp + self.item_fp
        return self.item_tp / d if d else 1.0

    @property
    def item_recall(self) -> float:
        d = self.item_tp + self.item_fn
        return self.item_tp / d if d else 1.0

    @property
    def item_f1(self) -> float:
        p, r = self.item_precision, self.item_recall
        return 2 * p * r / (p + r) if (p + r) else 1.0

    @property
    def fully_correct(self) -> bool:
        """Every scalar right, every row found, every matched field right."""
        if not all(self.scalars.values()):
            return False
        if self.item_fp or self.item_fn:
            return False
        return all(all(v) for v in self.item_fields.values())


def score_invoice(name: str, pred: Dict[str, Any], gold: Dict[str, Any]) -> InvoiceScore:
    s = InvoiceScore(name=name)

    for report_name, path, cmp in SCALAR_FIELDS:
        p, g = _dig(pred, path), _dig(gold, path)
        ok = bool(cmp(p, g))
        s.scalars[report_name] = ok
        if not ok:
            s.error_notes.append(f"{report_name}: pred={p!r} gold={g!r}")

    pred_items: Sequence[Dict[str, Any]] = pred.get("items") or []
    gold_items: Sequence[Dict[str, Any]] = gold.get("items") or []
    matched, unmatched_pred, unmatched_gold = match_items(pred_items, gold_items)

    s.item_tp = len(matched)
    s.item_fp = len(unmatched_pred)
    s.item_fn = len(unmatched_gold)
    s.item_fields = {fname: [] for fname, _ in ITEM_FIELDS}

    for pi, gi, _sim in matched:
        p_item, g_item = pred_items[pi], gold_items[gi]
        row_ok = True
        for fname, cmp in ITEM_FIELDS:
            ok = bool(cmp(p_item.get(fname), g_item.get(fname)))
            s.item_fields[fname].append(ok)
            if not ok:
                row_ok = False
                s.error_notes.append(
                    f"item[{gi}].{fname}: pred={p_item.get(fname)!r} "
                    f"gold={g_item.get(fname)!r}")
        if row_ok:
            s.rows_fully_correct += 1

    for j in unmatched_gold:
        s.error_notes.append(f"MISSED item: {gold_items[j].get('description')!r}")
    for i in unmatched_pred:
        s.error_notes.append(f"SPURIOUS item: {pred_items[i].get('description')!r}")

    return s


@dataclass
class Report:
    scores: List[InvoiceScore]

    @property
    def n(self) -> int:
        return len(self.scores)

    @property
    def fully_correct_rate(self) -> float:
        return sum(s.fully_correct for s in self.scores) / self.n if self.n else 0.0

    def scalar_accuracy(self) -> Dict[str, float]:
        out: Dict[str, float] = {}
        for report_name, _, _ in SCALAR_FIELDS:
            vals = [s.scalars.get(report_name, False) for s in self.scores]
            out[report_name] = sum(vals) / len(vals) if vals else 0.0
        return out

    def item_field_accuracy(self) -> Dict[str, Optional[float]]:
        out: Dict[str, Optional[float]] = {}
        for fname, _ in ITEM_FIELDS:
            vals = [v for s in self.scores for v in s.item_fields.get(fname, [])]
            out[fname] = (sum(vals) / len(vals)) if vals else None
        return out

    def item_prf(self) -> Dict[str, float]:
        tp = sum(s.item_tp for s in self.scores)
        fp = sum(s.item_fp for s in self.scores)
        fn = sum(s.item_fn for s in self.scores)
        p = tp / (tp + fp) if (tp + fp) else 1.0
        r = tp / (tp + fn) if (tp + fn) else 1.0
        f1 = 2 * p * r / (p + r) if (p + r) else 1.0
        return {"tp": tp, "fp": fp, "fn": fn,
                "precision": p, "recall": r, "f1": f1}


def _bar(x: Optional[float], width: int = 20) -> str:
    if x is None:
        return "-" * width
    filled = int(round(x * width))
    return "#" * filled + "." * (width - filled)


def format_report(report: Report, verbose: bool = False) -> str:
    lines: List[str] = []
    add = lines.append

    add("=" * 74)
    add(f"  HEADLINE   invoices fully correct: "
        f"{report.fully_correct_rate:6.1%}   "
        f"({sum(s.fully_correct for s in report.scores)}/{report.n})")
    add("=" * 74)

    add("")
    add("Scalar fields")
    add("-" * 74)
    for name, acc in report.scalar_accuracy().items():
        add(f"  {name:<22} {acc:6.1%}  {_bar(acc)}")

    prf = report.item_prf()
    add("")
    add("Line items -- detection")
    add("-" * 74)
    add(f"  precision {prf['precision']:6.1%}   recall {prf['recall']:6.1%}   "
        f"F1 {prf['f1']:6.1%}")
    add(f"  tp={prf['tp']}  fp={prf['fp']} (spurious)  fn={prf['fn']} (missed)")

    add("")
    add("Line items -- field accuracy (over matched rows only)")
    add("-" * 74)
    for name, acc in report.item_field_accuracy().items():
        if acc is None:
            add(f"  {name:<22}      -  (no matched rows)")
        else:
            add(f"  {name:<22} {acc:6.1%}  {_bar(acc)}")

    add("")
    add("Per invoice")
    add("-" * 74)
    add(f"  {'invoice':<22} {'ok':<4} {'items P/R/F1':<22} scalars")
    for s in sorted(report.scores, key=lambda x: x.name):
        n_ok = sum(s.scalars.values())
        n_tot = len(s.scalars)
        flag = "PASS" if s.fully_correct else "FAIL"
        prf_s = f"{s.item_precision:.2f}/{s.item_recall:.2f}/{s.item_f1:.2f}"
        add(f"  {s.name:<22} {flag:<4} {prf_s:<22} {n_ok}/{n_tot}")

    if verbose:
        add("")
        add("Failures")
        add("-" * 74)
        for s in sorted(report.scores, key=lambda x: x.name):
            if s.error_notes:
                add(f"  {s.name}")
                for note in s.error_notes:
                    add(f"      {note}")

    return "\n".join(lines)
