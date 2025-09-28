# scripts/extract_tables_pipeline.py
import argparse
from pathlib import Path
from scripts.photo_to_items import PhotoToItemsPipeline, PipelineConfig, _to_json
from core.io_utils import ensure_dir, save_json


def build_cfg_from_args(args) -> PipelineConfig:
    return PipelineConfig(
        det_model_id=args.det_model_id,
        det_threshold=args.det_threshold,
        ocr_lang=args.lang,
        ocr_psm=args.psm,
        ocr_oem=args.oem,
        ocr_preprocess=not args.no_preprocess,
        tesseract_cmd=args.tesseract_cmd,
        header_rows=args.header_rows,
        row_gap_px=args.row_gap,
        locale_hint=args.locale,
        save_overlays=not args.no_overlays,
        save_crops=not args.no_crops,
    )


def main():
    ap = argparse.ArgumentParser(
        description="One-shot: photo → table detection → OCR → line items JSON.")
    ap.add_argument("--inputs", nargs="+", required=True,
                    help="Invoice photo(s): jpg/png.")
    ap.add_argument("--outdir", default="data/outputs",
                    help="Output root directory.")
    # detection
    ap.add_argument("--det-model-id",
                    default="microsoft/table-transformer-detection")
    ap.add_argument("--det-threshold", type=float, default=0.5)
    # OCR
    ap.add_argument("--lang", default="eng",
                    help="Tesseract lang, e.g., eng or eng+vie")
    ap.add_argument("--psm", type=int, default=6,
                    help="Tesseract page segmentation mode")
    ap.add_argument("--oem", type=int, default=3, help="Tesseract engine mode")
    ap.add_argument("--tesseract-cmd", default=None,
                    help="Path to tesseract executable (Windows)")
    ap.add_argument("--no-preprocess", action="store_true",
                    help="Disable OCR preprocessing")
    # parsing
    ap.add_argument("--header-rows", type=int, default=1)
    ap.add_argument("--row-gap", type=float, default=18.0)
    ap.add_argument("--locale", default="en_US")
    # outputs
    ap.add_argument("--no-overlays", action="store_true")
    ap.add_argument("--no-crops", action="store_true")

    args = ap.parse_args()
    cfg = build_cfg_from_args(args)
    pipe = PhotoToItemsPipeline(cfg)

    out_root = Path(args.outdir)
    ensure_dir(out_root / "pipeline_json")

    for p in args.inputs:
        res = pipe.run_on_image(p, out_root)
        out_path = out_root / "pipeline_json" / \
            (Path(p).stem + "_pipeline.json")
        save_json(_to_json(res), out_path)
        print(f"[OK] {Path(p).name} → {out_path}")


if __name__ == "__main__":
    main()
