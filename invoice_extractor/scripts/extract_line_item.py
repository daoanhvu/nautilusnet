# scripts/extract_line.py
import argparse
import json
from pathlib import Path
from typing import List, Dict, Any
from PIL import Image

from ocr.tesseract_ocr import OCR
from scripts.rowcol_infer import cluster_rows, infer_column_boundaries, assign_tokens_to_columns
from scripts.parse_items import guess_column_roles, parse_rows_to_items, reconcile_totals


def main():
    pass


if __name__ == "__main__":
    main()
