# scripts/extract_line.py
import argparse
import json
from pathlib import Path
from typing import List, Dict, Any
from PIL import Image

from ocr.tesseract_ocr import OCR
from scripts.rowcol_infer import cluster_rows, infer_column_boundaries, assign_tokens_to_columns
from scripts.parse_items import guess_column_roles, parse_rows_to_items, reconcile_totals


def save_json(path: Path, obj: Dict[str, Any]) -> None:
    with open(path, "w", encoding="utf-8") as f:
        json.dump(obj, f, indent=2, ensure_ascii=False)


def main():
    ap = argparse.ArgumentParser(
        description="Extract line items from a table crop image.")
    ap.add_argument("--inputs", nargs="+", required=True,
                    help="Path(s) to table crop images (jpg/png).")
    ap.add_argument("--outdir", default="data/outputs/line_items",
                    help="Where to save JSON outputs.")
    ap.add_argument("--header-rows", type=int, default=1,
                    help="Number of header rows to use for column inference.")
    ap.add_argument("--row-gap", type=float, default=18.0,
                    help="Max y-gap (px) for tokens to belong to the same row.")
    ap.add_argument("--locale", default="en_US",
                    help="Locale hint for number parsing.")
    args = ap.parse_args()

    outdir = Path(args.outdir)
    outdir.mkdir(parents=True, exist_ok=True)
    ocr = OCR()

    for p in args.inputs:
        img_path = Path(p)
        image = Image.open(img_path).convert("RGB")
        tokens = ocr.run(image)
        if not tokens:
            save_json(outdir / f"{img_path.stem}_items.json",
                      {"image": str(img_path), "items": [], "note": "no OCR tokens"})
            continue

        # 1) Row clustering
        row_token_idxs = cluster_rows(tokens, distance_px=args.row_gap)
        # Build row token lists
        rows_tokens: List[List[Dict[str, Any]]] = [[tokens[i]
                                                    for i in idxs] for idxs in row_token_idxs]

        # 2) Column inference from header rows
        header_tokens = [
            t for r in rows_tokens[:max(1, args.header_rows)] for t in r]
        columns = infer_column_boundaries(header_tokens, min_gap_px=25.0)

        # 3) Assign tokens to columns for all rows (skip header when parsing)
        rows_columns: List[List[List[Dict[str, Any]]]] = []
        for r in rows_tokens[args.header_rows:]:
            col_bins = assign_tokens_to_columns(r, columns)
            rows_columns.append(col_bins)

        # 4) Guess column roles from header row text
        if rows_tokens and args.header_rows > 0:
            header_cols = assign_tokens_to_columns(rows_tokens[0], columns)
            header_texts = [" ".join([t["text"] for t in col])
                            for col in header_cols]
        else:
            header_texts = []
        roles = guess_column_roles(header_texts)

        # 5) Parse rows → items + reconciliation
        items = parse_rows_to_items(
            rows_columns, roles, locale_hint=args.locale)
        recon = reconcile_totals(items)

        save_json(outdir / f"{img_path.stem}_items.json", {
            "image": str(img_path),
            "columns": columns,
            "roles": roles,
            "items": items,
            "reconciliation": recon
        })
        print(
            f"[OK] {img_path.name} → {len(items)} items | roles={roles} | recon={recon}")


if __name__ == "__main__":
    main()
