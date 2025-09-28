# scripts/total_detect.py
from __future__ import annotations
from typing import List, Dict, Any, Optional, Tuple
from dataclasses import dataclass
from PIL import Image
import math

from ocr.tesseract_ocr import OCR
from scripts.rowcol_infer import cluster_rows, assign_tokens_to_columns, infer_column_boundaries
from scripts.parse_items import _as_number as parse_number


@dataclass
class TotalDetectionConfig:
    # only scan bottom portion for speed/precision; 0 < bottom_scan_ratio <= 1
    bottom_scan_ratio: float = 0.55
    # y-gap for row clustering
    row_gap_px: float = 22.0
    # how far (px) we allow the amount to be from the keyword, to the right
    max_right_dx: float = 600.0
    # locales for number parsing (first is primary)
    locale_hints: Tuple[str, ...] = ("en_US", "vi_VN", "de_DE")
    # Treat these as “strong” total keys (higher score)
    total_keys: Tuple[str, ...] = (
        "grand total", "amount due", "balance due", "total due", "total amount", "total"
    )
    # Common variants for subtotal
    subtotal_keys: Tuple[str, ...] = (
        "subtotal", "sub total", "tạm tính", "tong tam", "tạm thu", "interim total"
    )
    # Common tax keys
    tax_keys: Tuple[str, ...] = (
        "tax", "vat", "gst", "pst", "hst", "sales tax", "thuế", "thuế gtgt", "vat 10", "vat 8"
    )
    # Shipping / discount keys (optional extraction)
    misc_amount_keys: Tuple[str, ...] = (
        "shipping", "discount", "delivery", "handling", "fee")
    currency_hints: Tuple[str, ...] = (
        "$", "€", "£", "¥", "₫", "usd", "eur", "gbp", "jpy", "vnd")


@dataclass
class TotalSection:
    # bounding box covering detected rows (x0,y0,x1,y1) in full image coords
    box: List[int]
    # subtotal, tax_amount, total, amount_due, etc.
    fields: Dict[str, Optional[float]]
    raw_lines: List[str]                 # the matched lines for debugging
    confidence: float


def _lower(s: str) -> str:
    return s.lower().strip()


def _contains_any(s: str, keys: Tuple[str, ...]) -> bool:
    sl = _lower(s)
    return any(k in sl for k in keys)


def _right_amount_candidate(row_tokens: List[Dict[str, Any]], col_hint: Optional[Tuple[float, float]]) -> Optional[str]:
    """
    Pick the amount on the row preferably to the right side.
    If columns are known, prefer the rightmost column text; else pick the rightmost token that looks numeric.
    """
    if not row_tokens:
        return None
    # If we were given a right-side column span, collect tokens whose centers fall inside it
    if col_hint is not None:
        l, r = col_hint
        right_tokens = [t for t in row_tokens if (
            0.5*(t["box"][0]+t["box"][2]) >= l and 0.5*(t["box"][0]+t["box"][2]) <= r)]
        if right_tokens:
            txt = " ".join(t["text"] for t in sorted(
                right_tokens, key=lambda t: t["box"][0]))
            return txt.strip() or None
    # Fallback: take the rightmost numeric-ish token span
    numericish = [t for t in row_tokens if any(c.isdigit() for c in t["text"])]
    if not numericish:
        return None
    numericish.sort(key=lambda t: t["box"][0])  # left->right
    txt = numericish[-1]["text"]
    return txt.strip() or None


def _row_text(row_tokens: List[Dict[str, Any]]) -> str:
    return " ".join(t["text"] for t in sorted(row_tokens, key=lambda t: t["box"][0])).strip()


def detect_total_section(
    image: Image.Image,
    ocr: OCR,
    table_boxes: Optional[List[List[int]]] = None,
    cfg: Optional[TotalDetectionConfig] = None
) -> Optional[TotalSection]:
    """
    Heuristic detector:
      - OCR bottom X% of the image
      - cluster rows, infer rough columns from bottom headers if any
      - look for lines containing subtotal / tax / total / amount due keywords
      - for each matched line, pick the amount to the right
      - produce a union bbox covering matched lines and a confidence score
    """
    if cfg is None:
        cfg = TotalDetectionConfig()

    W, H = image.size
    y0_scan = int((1.0 - cfg.bottom_scan_ratio) * H)
    crop = image.crop((0, y0_scan, W, H))

    tokens = ocr.run(crop)
    if not tokens:
        return None

    # Shift tokens' boxes to full-image coordinates
    for t in tokens:
        t["box"][1] += y0_scan
        t["box"][3] += y0_scan

    # Cluster into rows
    rows_idx = cluster_rows(tokens, distance_px=cfg.row_gap_px)
    rows_tokens = [[tokens[i] for i in idxs] for idxs in rows_idx]

    # Try to infer coarse columns (helps pick right-side amounts)
    header_tokens = [tok for r in rows_tokens[:1]
                     for tok in r] if rows_tokens else []
    columns = infer_column_boundaries(header_tokens, min_gap_px=30.0)
    # assume right third is amount column
    right_col = columns[-1] if columns else (0.66*W, W)

    # Assemble candidate lines
    # (field_key, row_idx, raw_text, parsed_amount)
    matches: List[Tuple[str, int, str, Optional[float]]] = []
    matched_rows: List[int] = []
    raw_lines: List[str] = []

    def try_row(row_idx: int, alias: str, keyset: Tuple[str, ...]):
        row = rows_tokens[row_idx]
        text = _row_text(row)
        if not _contains_any(text, keyset):
            return
        raw_lines.append(text)
        # Amount preference: right side of the row
        amt_txt = _right_amount_candidate(row, right_col)
        parsed: Optional[float] = None
        if amt_txt:
            # try multiple locales
            for loc in cfg.locale_hints:
                parsed = parse_number(amt_txt, locale_hint=loc)
                if isinstance(parsed, float):
                    break
        matches.append((alias, row_idx, text, parsed))
        matched_rows.append(row_idx)

    # Pass 1: pick out lines by class
    for i in range(len(rows_tokens)):
        t = _row_text(rows_tokens[i]).lower()
        # prioritize explicit keys
        if _contains_any(t, cfg.subtotal_keys):
            try_row(i, "subtotal", cfg.subtotal_keys)
        if _contains_any(t, cfg.tax_keys):
            try_row(i, "tax_amount", cfg.tax_keys)
        if _contains_any(t, cfg.total_keys):
            try_row(i, "total", cfg.total_keys)
        if _contains_any(t, ("amount due", "balance due")):
            try_row(i, "amount_due", ("amount due", "balance due"))
        if _contains_any(t, cfg.misc_amount_keys):
            try_row(i, "misc", cfg.misc_amount_keys)

    if not matches:
        return None

    # Choose the most “down/right” total if multiple totals present
    totals = [(f, r, txt, val)
              for (f, r, txt, val) in matches if f in ("total", "amount_due")]
    if len(totals) > 1:
        totals.sort(key=lambda x: (rows_tokens[x[1]][0]["box"][1],   # larger y = nearer bottom
                                   max(tok["box"][0] for tok in rows_tokens[x[1]])))  # rightmost
        # keep the last (most bottom-right)
        chosen_total = totals[-1]
        # remove other total/amount_due entries
        matches = [m for m in matches if m is chosen_total or m[0]
                   not in ("total", "amount_due")]

    # Aggregate results
    fields: Dict[str, Optional[float]] = {}
    for alias, _, _, val in matches:
        if alias == "misc":
            # you might collect misc later per label; for now skip or aggregate
            continue
        # prefer amount_due over total if both present but total missing value
        if alias in fields and fields[alias] is not None:
            continue
        fields[alias] = val

    # Build a bounding box covering matched rows
    xs, ys = [], []
    for _, r, _, _ in matches:
        for tok in rows_tokens[r]:
            xs.extend([tok["box"][0], tok["box"][2]])
            ys.extend([tok["box"][1], tok["box"][3]])
    if xs and ys:
        x0, x1 = int(min(xs)), int(max(xs))
        y0, y1 = int(min(ys)), int(max(ys))
    else:
        x0 = y0 = 0
        x1 = W
        y1 = H

    # Confidence: combine heuristics
    # - keyword strength
    kw_score = 0.0
    for alias, _, text, val in matches:
        if alias == "total" or alias == "amount_due":
            kw_score += 1.0
        elif alias == "subtotal":
            kw_score += 0.6
        elif alias == "tax_amount":
            kw_score += 0.6
    kw_score = min(1.5, kw_score)

    # - bottom bias (closer to bottom = higher)
    bottom_bias = (y1 / H) ** 0.6

    # - currency/number presence
    has_currency = any(any(cur in _row_text(rows_tokens[r]).lower() for cur in cfg.currency_hints)
                       for _, r, _, _ in matches)
    cur_bonus = 0.2 if has_currency else 0.0

    confidence = max(0.0, min(1.0, 0.4*kw_score + 0.5*bottom_bias + cur_bonus))

    # Fill small derived helpers (optional)
    if "amount_due" not in fields and "total" in fields:
        fields.setdefault("amount_due", fields.get("total"))

    return TotalSection(
        box=[x0, y0, x1, y1],
        fields=fields,
        raw_lines=raw_lines,
        confidence=float(confidence)
    )
