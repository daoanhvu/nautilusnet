# scripts/detect_tables.py
import argparse
from pathlib import Path
from tqdm import tqdm
from models.table_detector import TableRegionDetector
from core.io_utils import load_image, ensure_dir, save_json
from core.viz import draw_boxes


def main():
    parser = argparse.ArgumentParser(
        description="Detect table regions in invoice photos.")
    parser.add_argument("--inputs", nargs="+", required=True,
                        help="Path(s) to invoice image(s) (jpg/png).")
    parser.add_argument("--outdir", default="data/outputs",
                        help="Output directory root.")
    parser.add_argument("--score-threshold", type=float,
                        default=0.75, help="Detection confidence threshold.")
    args = parser.parse_args()

    out_root = Path(args.outdir)
    out_json_dir = ensure_dir(out_root / "tables_json")
    out_crops_dir = ensure_dir(out_root / "table_crops")
    out_overlay_dir = ensure_dir(out_root / "overlays")

    detector = TableRegionDetector(score_threshold=args.score_threshold)

    for img_path in tqdm(args.inputs, desc="Processing"):
        img_path = Path(img_path)
        image = load_image(img_path)

        detections = detector.predict(image)

        # Save JSON
        json_obj = {"image": str(img_path), "detections": detections}
        save_json(json_obj, out_json_dir / f"{img_path.stem}_tables.json")

        # Save overlay
        overlay = draw_boxes(image, detections)
        overlay.save(out_overlay_dir /
                     f"{img_path.stem}_overlay.jpg", quality=95)

        # Save crops
        crops = detector.crop_tables(image, detections)
        for idx, (crop, meta) in enumerate(crops):
            crop_name = f"{img_path.stem}_table_{idx:02d}.png"
            crop.save(out_crops_dir / crop_name, quality=95)

    print(
        f"Done. JSON → {out_json_dir}, crops → {out_crops_dir}, overlays → {out_overlay_dir}")


if __name__ == "__main__":
    main()
