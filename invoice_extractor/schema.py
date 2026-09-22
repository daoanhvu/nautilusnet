"""Frozen extraction schema.

Ground truth labels, model predictions, and the evaluation harness all speak
this one shape. Changing it invalidates existing labels, so change it
deliberately.

Labeling policy (see docs/labeling_policy.md for the reasoning):
  1. items[] holds only things bought or charged. Discount rows and tax rows go
     to totals, even when printed inside the table body.
  2. Record only values printed on the page. Never compute a missing one --
     a null that the validator can derive is more useful than a guess.
  3. Money is a float in the invoice currency. Parenthesised amounts, e.g.
     "(50.00)", are negative.
  4. description is the item's NAME. Any secondary prose goes in details.
  5. unit is separated out of the quantity cell: "10 pcs" -> qty 10, unit "pcs".
"""
from __future__ import annotations

from dataclasses import dataclass, field, asdict
from typing import Any, Dict, List, Optional


@dataclass
class Logo:
    # [x0, y0, x1, y1] in original image pixels
    box: Optional[List[int]] = None
    # Resolved identity from the vendor gallery; None when unknown/unmatched.
    vendor_id: Optional[str] = None
    match_score: float = 0.0
    # False for template-vendor marks (e.g. the "OnlineInvoices" logo on
    # invoice-002) that are NOT the issuing organisation's logo.
    belongs_to_issuer: Optional[bool] = None


@dataclass
class Party:
    name: Optional[str] = None
    address: Optional[str] = None
    tax_id: Optional[str] = None
    phone: Optional[str] = None
    email: Optional[str] = None


@dataclass
class LineItem:
    description: Optional[str] = None      # the item name
    details: Optional[str] = None          # secondary prose, when a separate column
    sku: Optional[str] = None              # item code / HS code / part number
    unit: Optional[str] = None             # "ea", "hour", "pcs", ...
    quantity: Optional[float] = None
    unit_price: Optional[float] = None
    line_total: Optional[float] = None
    confidence: float = 1.0


@dataclass
class TaxLine:
    label: Optional[str] = None            # "GST 10%", "Sales Tax 5.0%", "Tax (8%)"
    rate: Optional[float] = None           # 0.10 for 10%
    base: Optional[float] = None           # the amount taxed, when stated
    amount: Optional[float] = None


@dataclass
class Totals:
    subtotal: Optional[float] = None
    discount: Optional[float] = None       # positive magnitude of the reduction
    tax_lines: List[TaxLine] = field(default_factory=list)
    tax_total: Optional[float] = None      # sum of tax_lines amounts
    total: Optional[float] = None
    amount_due: Optional[float] = None


@dataclass
class Validation:
    checks_passed: List[str] = field(default_factory=list)
    checks_failed: List[str] = field(default_factory=list)
    needs_review: bool = False


@dataclass
class Invoice:
    image: Optional[str] = None
    seller: Party = field(default_factory=Party)
    buyer: Party = field(default_factory=Party)
    logo: Logo = field(default_factory=Logo)

    number: Optional[str] = None
    issue_date: Optional[str] = None       # ISO 8601 (YYYY-MM-DD) when parseable
    due_date: Optional[str] = None
    currency: Optional[str] = None         # ISO 4217 when determinable

    items: List[LineItem] = field(default_factory=list)
    item_count: Optional[int] = None       # stated count; defaults to len(items)

    totals: Totals = field(default_factory=Totals)
    validation: Validation = field(default_factory=Validation)
    notes: Optional[str] = None            # labeler notes; never scored

    def __post_init__(self) -> None:
        if self.item_count is None:
            self.item_count = len(self.items)

    def to_dict(self) -> Dict[str, Any]:
        return asdict(self)


# --------------------------------------------------------------------------
# Deserialisation. Explicit rather than generic so that an unexpected key in a
# hand-written label file fails loudly instead of being silently dropped.
# --------------------------------------------------------------------------

def _build(cls, data: Optional[Dict[str, Any]]):
    if not data:
        return cls()
    known = {f for f in cls.__dataclass_fields__}
    unknown = set(data) - known
    if unknown:
        raise ValueError(f"{cls.__name__}: unknown field(s) {sorted(unknown)}")
    return cls(**data)


def invoice_from_dict(data: Dict[str, Any]) -> Invoice:
    totals_raw = dict(data.get("totals") or {})
    tax_lines = [_build(TaxLine, t) for t in totals_raw.pop("tax_lines", [])]
    totals = _build(Totals, totals_raw)
    totals.tax_lines = tax_lines

    return Invoice(
        image=data.get("image"),
        seller=_build(Party, data.get("seller")),
        buyer=_build(Party, data.get("buyer")),
        logo=_build(Logo, data.get("logo")),
        number=data.get("number"),
        issue_date=data.get("issue_date"),
        due_date=data.get("due_date"),
        currency=data.get("currency"),
        items=[_build(LineItem, it) for it in data.get("items", [])],
        item_count=data.get("item_count"),
        totals=totals,
        validation=_build(Validation, data.get("validation")),
        notes=data.get("notes"),
    )


# --------------------------------------------------------------------------
# JSON Schema -- used later for the extractor's structured output
# (output_config.format) so the model cannot emit an off-shape response.
# --------------------------------------------------------------------------

def _nullable(*types: str) -> Dict[str, Any]:
    return {"type": [*types, "null"]}


PARTY_SCHEMA = {
    "type": "object",
    "additionalProperties": False,
    "properties": {
        "name": _nullable("string"),
        "address": _nullable("string"),
        "tax_id": _nullable("string"),
        "phone": _nullable("string"),
        "email": _nullable("string"),
    },
    "required": ["name", "address", "tax_id", "phone", "email"],
}

INVOICE_JSON_SCHEMA: Dict[str, Any] = {
    "type": "object",
    "additionalProperties": False,
    "properties": {
        "seller": PARTY_SCHEMA,
        "buyer": PARTY_SCHEMA,
        "number": _nullable("string"),
        "issue_date": _nullable("string"),
        "due_date": _nullable("string"),
        "currency": _nullable("string"),
        "items": {
            "type": "array",
            "items": {
                "type": "object",
                "additionalProperties": False,
                "properties": {
                    "description": _nullable("string"),
                    "details": _nullable("string"),
                    "sku": _nullable("string"),
                    "unit": _nullable("string"),
                    "quantity": _nullable("number"),
                    "unit_price": _nullable("number"),
                    "line_total": _nullable("number"),
                },
                "required": ["description", "details", "sku", "unit",
                             "quantity", "unit_price", "line_total"],
            },
        },
        "item_count": _nullable("integer"),
        "totals": {
            "type": "object",
            "additionalProperties": False,
            "properties": {
                "subtotal": _nullable("number"),
                "discount": _nullable("number"),
                "tax_lines": {
                    "type": "array",
                    "items": {
                        "type": "object",
                        "additionalProperties": False,
                        "properties": {
                            "label": _nullable("string"),
                            "rate": _nullable("number"),
                            "base": _nullable("number"),
                            "amount": _nullable("number"),
                        },
                        "required": ["label", "rate", "base", "amount"],
                    },
                },
                "tax_total": _nullable("number"),
                "total": _nullable("number"),
                "amount_due": _nullable("number"),
            },
            "required": ["subtotal", "discount", "tax_lines", "tax_total",
                         "total", "amount_due"],
        },
    },
    "required": ["seller", "buyer", "number", "issue_date", "due_date",
                 "currency", "items", "item_count", "totals"],
}
