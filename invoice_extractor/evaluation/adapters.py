"""Adapters from a pipeline's native output into the frozen schema.

Each extractor gets an adapter here, so the harness can score any of them
against the same labels. Adapters must not invent values: a field the pipeline
does not produce stays None, which is scored as a miss.
"""
from __future__ import annotations

from typing import Any, Callable, Dict


def from_legacy_pipeline(raw: Dict[str, Any]) -> Dict[str, Any]:
    """Output of scripts/photo_to_items.py -> schema dict.

    The legacy pipeline extracts neither the seller nor the invoice number, so
    those stay null by design.
    """
    items = []
    for table in raw.get("tables") or []:
        for it in table.get("items") or []:
            items.append({
                "description": it.get("description"),
                "details": None,
                "sku": None,
                "unit": None,                      # legacy has no unit concept
                "quantity": it.get("qty"),
                "unit_price": it.get("unit_price"),
                "line_total": it.get("line_total"),
                "confidence": it.get("confidence", 0.0),
            })

    fields = ((raw.get("totals") or {}).get("fields")) or {}

    return {
        "image": raw.get("image"),
        "seller": {"name": None, "address": None, "tax_id": None,
                   "phone": None, "email": None},
        "buyer": {"name": None, "address": None, "tax_id": None,
                  "phone": None, "email": None},
        "number": None,
        "issue_date": None,
        "due_date": None,
        "currency": None,
        "items": items,
        "item_count": len(items),
        "totals": {
            "subtotal": fields.get("subtotal"),
            "discount": None,
            "tax_lines": [],
            "tax_total": fields.get("tax_amount"),
            "total": fields.get("total"),
            "amount_due": fields.get("amount_due"),
        },
    }


def from_native(raw: Dict[str, Any]) -> Dict[str, Any]:
    """Already in the frozen schema -- pass through."""
    return raw


def empty_prediction() -> Dict[str, Any]:
    """Used when a prediction file is missing, so a crashed run is scored as a
    total miss rather than silently excluded from the average."""
    return {
        "seller": {}, "buyer": {}, "number": None, "issue_date": None,
        "due_date": None, "currency": None, "items": [], "item_count": 0,
        "totals": {"subtotal": None, "discount": None, "tax_lines": [],
                   "tax_total": None, "total": None, "amount_due": None},
    }


ADAPTERS: Dict[str, Callable[[Dict[str, Any]], Dict[str, Any]]] = {
    "legacy": from_legacy_pipeline,
    "native": from_native,
}
