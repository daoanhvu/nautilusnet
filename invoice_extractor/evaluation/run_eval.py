"""Score a set of predictions against the labelled test set.

    python evaluation/run_eval.py --pred data/outputs/pipeline_json --adapter legacy -v

Run from the invoice_extractor/ directory (same convention as
scripts/extract_tables_pipeline.py).
"""
from __future__ import annotations

import argparse
import json
import sys
from pathlib import Path
from typing import Any, Dict, Optional

sys.path.insert(0, str(Path(__file__).resolve().parent.parent))

from evaluation.adapters import ADAPTERS, empty_prediction   # noqa: E402
from evaluation.metrics import Report, format_report, score_invoice  # noqa: E402
from schema import invoice_from_dict                          # noqa: E402


def load_json(path: Path) -> Dict[str, Any]:
    with open(path, "r", encoding="utf-8") as f:
        return json.load(f)


def find_prediction(pred_dir: Path, stem: str) -> Optional[Path]:
    """Match a label stem to a prediction file, tolerating suffixes such as
    'invoice-001' -> 'invoice-001_pipeline.json'."""
    exact = pred_dir / f"{stem}.json"
    if exact.exists():
        return exact
    candidates = sorted(pred_dir.glob(f"{stem}*.json"))
    return candidates[0] if candidates else None


def main() -> int:
    ap = argparse.ArgumentParser(description="Score invoice extraction against labels.")
    ap.add_argument("--labels", default="data/labels",
                    help="Directory of ground-truth JSON files.")
    ap.add_argument("--pred", required=True,
                    help="Directory of prediction JSON files.")
    ap.add_argument("--adapter", default="native", choices=sorted(ADAPTERS),
                    help="How to read the prediction files.")
    ap.add_argument("-v", "--verbose", action="store_true",
                    help="List every field-level failure.")
    ap.add_argument("--json-out", default=None,
                    help="Also write the raw scores to this path.")
    args = ap.parse_args()

    labels_dir, pred_dir = Path(args.labels), Path(args.pred)
    if not labels_dir.is_dir():
        print(f"error: labels directory not found: {labels_dir}", file=sys.stderr)
        return 2
    if not pred_dir.is_dir():
        print(f"error: predictions directory not found: {pred_dir}", file=sys.stderr)
        return 2

    label_files = sorted(labels_dir.glob("*.json"))
    if not label_files:
        print(f"error: no label files in {labels_dir}", file=sys.stderr)
        return 2

    adapt = ADAPTERS[args.adapter]
    scores, missing = [], []

    for lf in label_files:
        stem = lf.stem
        gold_obj = load_json(lf)
        gold = invoice_from_dict(gold_obj).to_dict()   # validates the label file

        pf = find_prediction(pred_dir, stem)
        if pf is None:
            missing.append(stem)
            pred = empty_prediction()
        else:
            pred = adapt(load_json(pf))

        scores.append(score_invoice(stem, pred, gold))

    report = Report(scores=scores)
    print(format_report(report, verbose=args.verbose))

    if missing:
        print("")
        print(f"NOTE: {len(missing)} of {len(label_files)} predictions were missing "
              f"and scored as empty: {', '.join(missing)}")

    if args.json_out:
        out = {
            "n": report.n,
            "fully_correct_rate": report.fully_correct_rate,
            "scalar_accuracy": report.scalar_accuracy(),
            "item_prf": report.item_prf(),
            "item_field_accuracy": report.item_field_accuracy(),
            "missing_predictions": missing,
            "per_invoice": [
                {"name": s.name, "fully_correct": s.fully_correct,
                 "scalars": s.scalars, "item_f1": s.item_f1,
                 "errors": s.error_notes}
                for s in report.scores
            ],
        }
        Path(args.json_out).parent.mkdir(parents=True, exist_ok=True)
        with open(args.json_out, "w", encoding="utf-8") as f:
            json.dump(out, f, indent=2, ensure_ascii=False)
        print(f"\nScores written to {args.json_out}")

    return 0


if __name__ == "__main__":
    raise SystemExit(main())
