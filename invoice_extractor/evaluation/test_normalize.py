"""Self-checking tests for the comparison layer.

Plain asserts, no test framework needed:

    python evaluation/test_normalize.py

Every score the project ever reports passes through these functions, so a bug
here silently corrupts every result. Add a case whenever a real invoice
surprises you.
"""
from __future__ import annotations

import sys
from pathlib import Path

sys.path.insert(0, str(Path(__file__).resolve().parent.parent))

from evaluation.matching import match_items                      # noqa: E402
from evaluation.normalize import (                               # noqa: E402
    money_equal, norm_unit, parse_money, parse_qty, text_equal,
)

failures = []


def check(label, got, want):
    if got != want:
        failures.append(f"{label}: got {got!r}, want {want!r}")


# -- money ----------------------------------------------------------------
check("plain",        parse_money("200.00"),      200.00)
check("currency",     parse_money("$1,234.56"),   1234.56)
check("aud suffix",   parse_money("2,510.00 AUD"), 2510.00)
check("european",     parse_money("1.234,56"),    1234.56)
check("decimal comma", parse_money("12,34"),      12.34)
check("parens neg",   parse_money("(50.00)"),     -50.00)
check("minus",        parse_money("-50.00"),      -50.00)
check("thousands",    parse_money("5,000"),       5000.00)
check("dong",         parse_money("1.500.000₫"),  1500000.00)
check("none",         parse_money(None),          None)
check("empty",        parse_money(""),            None)
check("garbage",      parse_money("n/a"),         None)
check("float in",     parse_money(2510.0),        2510.00)

# -- quantity -------------------------------------------------------------
check("qty plain",    parse_qty("5"),      5.0)
check("qty w/ unit",  parse_qty("10 pcs"), 10.0)
check("qty hour",     parse_qty("1 hour"), 1.0)
check("qty none",     parse_qty(None),     None)

# -- units ----------------------------------------------------------------
check("unit ea",      norm_unit("ea"),     "ea")
check("unit each",    norm_unit("each"),   "ea")
check("unit pcs",     norm_unit("pcs"),    "ea")
check("unit from qty", norm_unit("10 pcs"), "ea")
check("unit hour",    norm_unit("hour"),   "hour")
check("unit hrs",     norm_unit("hrs"),    "hour")
check("unit none",    norm_unit(None),     None)
check("unit numeric only", norm_unit("10"), None)

# -- null semantics: absent must never silently equal present -------------
check("null==null",   money_equal(None, None),   True)
check("null!=value",  money_equal(None, 100.0),  False)
check("value!=null",  money_equal(100.0, None),  False)
check("tolerance",    money_equal(100.001, 100.0), True)
check("beyond tol",   money_equal(100.02, 100.0), False)

# -- text -----------------------------------------------------------------
check("case",         text_equal("Service Fee", "service fee"), True)
check("whitespace",   text_equal("Sample  product", "Sample product"), True)
check("edge punct",   text_equal("Subtotal:", "Subtotal"), True)
check("different",    text_equal("Tax", "Taxes"), False)
check("text null",    text_equal(None, None), True)
check("text null vs", text_equal(None, "x"), False)

# -- line-item matching ---------------------------------------------------
gold = [
    {"description": "Front and rear brake cables", "quantity": 1,
     "unit_price": 100.0, "line_total": 100.0},
    {"description": "New set of pedal arms", "quantity": 2,
     "unit_price": 25.0, "line_total": 50.0},
    {"description": "Labor 3hrs", "quantity": 3,
     "unit_price": 15.0, "line_total": 45.0},
]
# Same rows, shuffled, with OCR damage on one description.
pred = [
    {"description": "Labor 3hrs", "quantity": 3,
     "unit_price": 15.0, "line_total": 45.0},
    {"description": "Front and rear brake cabIes", "quantity": 1,
     "unit_price": 100.0, "line_total": 100.0},
    {"description": "New set of pedal arms", "quantity": 2,
     "unit_price": 25.0, "line_total": 50.0},
]
matched, up, ug = match_items(pred, gold)
check("reorder matches all", len(matched), 3)
check("no spurious", up, [])
check("no missed", ug, [])
pairs = {(p, g) for p, g, _ in matched}
check("labor aligned", (0, 2) in pairs, True)
check("brakes aligned", (1, 0) in pairs, True)

# A genuinely different row must not be force-matched.
matched2, up2, ug2 = match_items(
    [{"description": "Shipping surcharge", "line_total": 999.0}],
    [{"description": "Front and rear brake cables", "line_total": 100.0}],
)
check("unrelated not matched", len(matched2), 0)
check("counted as spurious", up2, [0])
check("counted as missed", ug2, [0])

# Empty prediction: everything missed, nothing spurious.
m3, up3, ug3 = match_items([], gold)
check("empty pred missed all", ug3, [0, 1, 2])
check("empty pred no fp", up3, [])

# -- report ---------------------------------------------------------------
if failures:
    print(f"FAILED ({len(failures)})")
    for f in failures:
        print("  " + f)
    raise SystemExit(1)
print("all normalization + matching checks passed")
