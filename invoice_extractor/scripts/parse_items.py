from typing import List, Dict, Any, Tuple, Optional
import re
from dateutil.parser import parse as parse_date  # if you later add date col
from babel.numbers import parse_decimal

_NUM = re.compile(r"[-+]?[\d.,]+")
_CURRENCY = re.compile(r"([$€£¥₫]|USD|EUR|GBP|JPY|VND)", re.I)


def _join_text(tokens: List[Dict[str, Any]]) -> str:
    return " ".join(t["text"] for t in tokens).strip()


def _is_numeric(s: str) -> bool:
    return bool(_NUM.fullmatch(s.replace(" ", "")))


def _as_number(s: str, locale_hint: str = "en_US") -> Optional[float]:
    s = s.strip()
    if not s:
        return None
    # remove currency symbols
    s = _CURRENCY.sub("", s)
    s = s.strip()
    try:
        # Heuristic: if both . and , present, assume European style when last sep is comma
        if "." in s and "," in s:
            last = s[max(s.rfind("."), s.rfind(","))]
            locale = "de_DE" if last == "," else locale_hint
        elif "," in s and s.count(",") == 1 and len(s.split(",")[-1]) == 2:
            # e.g., "12,34" -> decimal comma
            locale = "de_DE"
        else:
            locale = locale_hint
        return float(parse_decimal(s, locale=locale))
    except Exception:
        # fallback: stripping thousand separators
        s2 = s.replace(",", "")
        try:
            return float(s2)
        except Exception:
            return None


def guess_column_roles(column_texts: List[str]) -> List[str]:
    """
    Given header texts per column, guess roles: one of ['description','qty','unit_price','line_total','other']
    """
    roles = []
    for txt in column_texts:
        t = txt.lower()
        if any(k in t for k in ["qty", "quantity", "units", "pcs", "q'ty"]):
            roles.append("qty")
        elif any(k in t for k in ["unit price", "price", "rate", "u.p.", "unit cost"]):
            roles.append("unit_price")
        elif any(k in t for k in ["amount", "line total", "total", "ext price", "subtotal"]):
            roles.append("line_total")
        elif any(k in t for k in ["desc", "description", "item", "product", "name", "details"]):
            roles.append("description")
        else:
            roles.append("other")
    # Ensure at least one description
    if "description" not in roles and roles:
        roles[0] = "description"
    return roles


def parse_rows_to_items(
    rows: List[List[List[Dict[str, Any]]]],  # rows -> columns -> tokens
    roles: List[str],
    locale_hint: str = "en_US"
) -> List[Dict[str, Any]]:
    items = []
    for r_idx, cols in enumerate(rows):
        col_texts = [_join_text(c) for c in cols]
        raw_map = {roles[i] if i < len(
            roles) else f"c{i}": col_texts[i] for i in range(len(col_texts))}
        desc = ""
        qty = unit = total = None

        # Map by roles
        for i, role in enumerate(roles):
            if i >= len(col_texts):
                continue
            txt = col_texts[i]
            if role == "description":
                desc = txt
            elif role == "qty":
                qty = _as_number(txt, locale_hint)
            elif role == "unit_price":
                unit = _as_number(txt, locale_hint)
            elif role == "line_total":
                total = _as_number(txt, locale_hint)

        # Heuristics if roles were weak:
        if not desc:
            # take the longest text-ish column as description
            longest_i = max(range(len(col_texts)),
                            key=lambda k: len(col_texts[k]))
            desc = col_texts[longest_i]
        # Confidence: average OCR conf across tokens
        token_confs = [t["conf"] for c in cols for t in c] or [1.0]
        conf = float(sum(token_confs) / len(token_confs))
        items.append({
            "description": desc,
            "qty": qty,
            "unit_price": unit,
            "line_total": total,
            "raw_cols": raw_map,
            "confidence": conf
        })
    return items


def reconcile_totals(items: List[Dict[str, Any]], tol: float = 0.02) -> Dict[str, Any]:
    """
    Compute sums and sanity checks to support later validation.
    """
    sum_lines = sum([it["line_total"] for it in items if isinstance(
        it.get("line_total"), (int, float))] or [0.0])
    sum_est = 0.0
    # if line_total missing, try qty*unit
    for it in items:
        lt = it.get("line_total")
        if isinstance(lt, (int, float)):
            sum_est += lt
        else:
            q, u = it.get("qty"), it.get("unit_price")
            if isinstance(q, (int, float)) and isinstance(u, (int, float)):
                sum_est += q * u
    return {
        "sum_line_totals": round(sum_lines, 2),
        "sum_estimated": round(sum_est, 2),
        "within_tolerance": abs(sum_lines - sum_est) <= tol * max(1.0, sum_est)
    }
