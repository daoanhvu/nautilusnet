from __future__ import annotations
from pathlib import Path
from typing import Any, Dict, List
import json
from PIL import Image


def load_image(path: str | Path) -> Image.Image:
    return Image.open(path)


def ensure_dir(path: str | Path) -> Path:
    a_path = Path(path)
    a_path.mkdir(parents=True, exist_ok=True)
    return a_path


def save_json(obj: Dict[str, Any] | List[Dict[str, Any]], path: str | Path) -> None:
    with open(path, "w", encoding="utf-8") as f:
        json.dump(obj, f, indent=2, ensure_ascii=False)
