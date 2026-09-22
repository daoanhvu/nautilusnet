"""Value normalisation for comparison.

Every comparison in the harness goes through here. The rules are deliberately
conservative: two values match only when they mean the same thing, not merely
when they look similar. "$1,234.56" == "1234.56" is a match; "Tax" == "Taxes"
is not.
"""
from __future__ import annotations

import re
import unicodedata
from typing import Any, Optional

MONEY_TOL = 0.01      # absolute, in currency units
QTY_TOL = 1e-6

_CURRENCY = re.compile(r"[$€£¥₫]|\b(?:USD|EUR|GBP|JPY|VND|AUD|CAD|SGD)\b", re.I)
_WS = re.compile(r"\s+")
_PUNCT_EDGE = re.compile(r"^[\s\W_]+|[\s\W_]+$", re.UNICODE)

# Unit synonyms collapse to a canonical form so "1 ea" and "1 each" agree.
_UNIT_SYNONYMS = {
    "ea": "ea", "each": "ea", "pc": "ea", "pcs": "ea", "piece": "ea",
    "pieces": "ea", "unit": "ea", "units": "ea", "item": "ea", "items": "ea",
    "hr": "hour", "hrs": "hour", "hour": "hour", "hours": "hour",
    "day": "day", "days": "day",
    "kg": "kg", "kgs": "kg", "kilogram": "kg", "kilograms": "kg",
    "g": "g", "gram": "g", "grams": "g",
    "l": "l", "litre": "l", "litres": "l", "liter": "l", "liters": "l",
    "m": "m", "metre": "m", "metres": "m", "meter": "m", "meters": "m",
    "box": "box", "boxes": "box",
    "set": "set", "sets": "set",
    "month": "month", "months": "month",
}


def norm_text(value: Any) -> Optional[str]:
    """Casefold, strip edge punctuation, collapse whitespace.

    Returns None for null-ish input so that "absent" stays distinguishable
    from "present but empty".
    """
    if value is None:
        return None
    s = unicodedata.normalize("NFKC", str(value))
    s = _WS.sub(" ", s).strip()
    s = _PUNCT_EDGE.sub("", s)
    s = s.casefold()
    return s or None


def parse_money(value: Any) -> Optional[float]:
    """Parse a money value to float. Parenthesised amounts are negative.

    Handles "$1,234.56", "1.234,56" (European), "(50.00)", "2,510.00 AUD".
    """
    if value is None:
        return None
    if isinstance(value, (int, float)) and not isinstance(value, bool):
        return round(float(value), 2)

    s = unicodedata.normalize("NFKC", str(value)).strip()
    if not s:
        return None

    negative = False
    if s.startswith("(") and s.endswith(")"):
        negative, s = True, s[1:-1]
    s = _CURRENCY.sub("", s).strip()
    if s.startswith("-"):
        negative, s = True, s[1:].strip()
    s = s.replace(" ", "").replace(" ", "")
    if not s:
        return None

    # Decide which separator is the decimal point by looking at the last one.
    n_dot, n_comma = s.count("."), s.count(",")
    last_dot, last_comma = s.rfind("."), s.rfind(",")

    if n_dot and n_comma:
        if last_comma > last_dot:          # European: 1.234,56
            s = s.replace(".", "").replace(",", ".")
        else:                              # Anglo: 1,234.56
            s = s.replace(",", "")
    elif n_comma > 1:
        s = s.replace(",", "")             # 1,234,567 -- all thousands
    elif n_comma == 1:
        # A lone comma is a decimal separator only when it looks like one.
        tail = s[last_comma + 1:]
        s = s.replace(",", "." if len(tail) in (1, 2) else "")
    elif n_dot > 1:
        s = s.replace(".", "")             # 1.500.000 -- European thousands
    # A single dot is left as a decimal point. "1.500" is genuinely ambiguous
    # (1.5 or 1500); we take the Anglo reading, which is right for this corpus.

    try:
        out = float(s)
    except ValueError:
        return None
    return round(-out if negative else out, 2)


def parse_qty(value: Any) -> Optional[float]:
    """Parse a quantity, ignoring any unit glued onto it ("10 pcs" -> 10.0)."""
    if value is None:
        return None
    if isinstance(value, (int, float)) and not isinstance(value, bool):
        return float(value)
    m = re.search(r"[-+]?\d[\d.,]*", str(value))
    return parse_money(m.group(0)) if m else None


def norm_unit(value: Any) -> Optional[str]:
    """Canonicalise a unit, pulling it out of a combined cell if needed."""
    if value is None:
        return None
    s = norm_text(re.sub(r"[-+]?\d[\d.,]*", " ", str(value)))
    if not s:
        return None
    s = s.replace(".", "")
    return _UNIT_SYNONYMS.get(s, s)


def money_equal(a: Any, b: Any, tol: float = MONEY_TOL) -> bool:
    pa, pb = parse_money(a), parse_money(b)
    if pa is None or pb is None:
        return pa is None and pb is None
    return abs(pa - pb) <= tol


def qty_equal(a: Any, b: Any, tol: float = QTY_TOL) -> bool:
    pa, pb = parse_qty(a), parse_qty(b)
    if pa is None or pb is None:
        return pa is None and pb is None
    return abs(pa - pb) <= tol


def text_equal(a: Any, b: Any) -> bool:
    return norm_text(a) == norm_text(b)


def unit_equal(a: Any, b: Any) -> bool:
    return norm_unit(a) == norm_unit(b)


def int_equal(a: Any, b: Any) -> bool:
    try:
        ia = None if a is None else int(a)
        ib = None if b is None else int(b)
    except (TypeError, ValueError):
        return False
    return ia == ib


def text_similarity(a: Any, b: Any) -> float:
    """0..1 similarity on normalised text. Used only for line-item matching,
    never for scoring."""
    from difflib import SequenceMatcher
    na, nb = norm_text(a) or "", norm_text(b) or ""
    if not na and not nb:
        return 1.0
    if not na or not nb:
        return 0.0
    return SequenceMatcher(None, na, nb).ratio()
