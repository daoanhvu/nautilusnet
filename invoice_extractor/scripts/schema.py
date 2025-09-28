# scripts/schema.py

from typing import Optional, List, Dict, Any
from dataclasses import dataclass, asdict


@dataclass
class LineItem:
    description: str
    qty: Optional[float]
    unit_price: Optional[float]
    line_total: Optional[float]
    raw_cols: Dict[str, str]
    confidence: float


def as_json(item: LineItem) -> Dict[str, Any]:
    return asdict(item)
