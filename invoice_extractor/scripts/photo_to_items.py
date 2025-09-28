# scripts/photo_to_items.py
from __future__ import annotations
from typing import List, Dict, Any, Optional, Tuple
from pathlib import Path
from dataclasses import dataclass, asdict

from PIL import Image

from models.table_detector import TableRegionDetector
from ocr.tesseract_ocr import OCR as TesseractOCR
from scripts.rowcol_infer import cluster_rows, infer_column_boundaries, assign_tokens_to_columns
from scripts.parse_items import guess_column_roles, parse_rows_to_items, reconcile_totals
from scripts.total_detect import detect_total_section, TotalDetectionConfig, TotalSection
from core.viz import draw_boxes
from core.io_utils import ensure_dir, save_json


@dataclass
class PipelineConfig:
    # table detection
    det_model_id: str = "microsoft/table-transformer-detection"
    det_threshold: float = 0.5

    # OCR
    ocr_lang: str = "eng"
    ocr_psm: int = 6
    ocr_oem: int = 3
    ocr_preprocess: bool = True
    tesseract_cmd: Optional[str] = None   # windows path if needed

    # table parsing
    header_rows: int = 1
    row_gap_px: float = 18.0
    locale_hint: str = "en_US"

    # outputs
    save_overlays: bool = True
    save_crops: bool = True


@dataclass
class TableExtractionResult:
    table_index: int
    box: List[int]            # [x0,y0,x1,y1] in original image coords
    roles: List[str]          # column roles
    items: List[Dict[str, Any]]
    reconciliation: Dict[str, Any]
    crop_path: Optional[str] = None
    overlay_path: Optional[str] = None


@dataclass
class TotalsExtraction:
    box: List[int]
    fields: Dict[str, Optional[float]]
    raw_lines: List[str]
    confidence: float


@dataclass
class ImageExtractionResult:
    image_path: str
    width: int
    height: int
    tables: List[TableExtractionResult]
    totals: Optional[TotalsExtraction] = None


class PhotoToItemsPipeline:
    def __init__(self, cfg: PipelineConfig):
        self.cfg = cfg
        self.detector = TableRegionDetector(
            model_id=cfg.det_model_id,
            score_threshold=cfg.det_threshold
        )
        self.ocr = TesseractOCR(
            lang=cfg.ocr_lang,
            psm=cfg.ocr_psm,
            oem=cfg.ocr_oem,
            do_preprocess=cfg.ocr_preprocess,
            tesseract_cmd=cfg.tesseract_cmd
        )

    def run_on_image(
        self,
        image_path: str | Path,
        out_root: str | Path = "data/outputs",
    ) -> ImageExtractionResult:
        image_path = Path(image_path)
        out_root = Path(out_root)

        # Prepare dirs
        out_json_dir = ensure_dir(out_root / "pipeline_json")
        out_overlay_dir = ensure_dir(
            out_root / "overlays") if self.cfg.save_overlays else None
        out_crop_dir = ensure_dir(
            out_root / "table_crops") if self.cfg.save_crops else None

        # Load
        image = Image.open(image_path).convert("RGB")
        W, H = image.size

        # Detect table regions
        detections = self.detector.predict(image)

        # Optional: save detection overlay
        overlay_path = None
        if self.cfg.save_overlays:
            overlay = draw_boxes(image, detections)
            overlay_path = str(
                (out_overlay_dir / f"{image_path.stem}_det_overlay.jpg").resolve())
            overlay.save(overlay_path, quality=95)

        # Crop + process each table
        crops = self.detector.crop_tables(image, detections)
        tables_out: List[TableExtractionResult] = []

        for t_idx, (crop_img, meta) in enumerate(crops):
            # Save crop
            crop_path = None
            if self.cfg.save_crops:
                crop_path = str(
                    (out_crop_dir / f"{image_path.stem}_table_{t_idx:02d}.jpg").resolve())
                crop_img.save(crop_path, quality=95)

            # OCR
            tokens = self.ocr.run(crop_img)

            # If no tokens, skip
            if not tokens:
                tables_out.append(TableExtractionResult(
                    table_index=t_idx,
                    box=meta["box"],
                    roles=[],
                    items=[],
                    reconciliation={"sum_line_totals": 0.0,
                                    "sum_estimated": 0.0, "within_tolerance": True},
                    crop_path=crop_path
                ))
                continue

            # Row clustering (top→bottom rows, left→right tokens)
            row_token_idxs = cluster_rows(
                tokens, distance_px=self.cfg.row_gap_px)
            rows_tokens: List[List[Dict[str, Any]]] = [[tokens[i]
                                                        for i in idxs] for idxs in row_token_idxs]

            # Column boundaries from header rows
            header_tokens = [t for r in rows_tokens[:max(
                1, self.cfg.header_rows)] for t in r]
            columns = infer_column_boundaries(header_tokens, min_gap_px=25.0)

            # Assign tokens to columns
            rows_columns: List[List[List[Dict[str, Any]]]] = []
            for r in rows_tokens[self.cfg.header_rows:]:
                col_bins = assign_tokens_to_columns(r, columns)
                rows_columns.append(col_bins)

            # Guess roles from header text
            if rows_tokens and self.cfg.header_rows > 0:
                header_cols = assign_tokens_to_columns(rows_tokens[0], columns)
                header_texts = [" ".join([t["text"] for t in col])
                                for col in header_cols]
            else:
                header_texts = []
            roles = guess_column_roles(header_texts)

            # Parse rows to items
            items = parse_rows_to_items(
                rows_columns, roles, locale_hint=self.cfg.locale_hint)
            recon = reconcile_totals(items)

            tables_out.append(TableExtractionResult(
                table_index=t_idx,
                box=meta["box"],
                roles=roles,
                items=items,
                reconciliation=recon,
                crop_path=crop_path
            ))

        # Detect totals on full image (prefers bottom area)
        totals_cfg = TotalDetectionConfig(
            bottom_scan_ratio=0.55,      # scan bottom 55% of the page
            row_gap_px=max(18.0, self.cfg.row_gap_px),
            locale_hints=(self.cfg.locale_hint, "vi_VN", "en_US")
        )
        totals = detect_total_section(image, self.ocr, table_boxes=[
                                      t.box for t in tables_out] if tables_out else None, cfg=totals_cfg)

        totals_out = None
        if totals:
            totals_out = TotalsExtraction(
                box=totals.box,
                fields=totals.fields,
                raw_lines=totals.raw_lines,
                confidence=totals.confidence
            )

        # Build overall result
        result = ImageExtractionResult(
            image_path=str(image_path.resolve()),
            width=W, height=H,
            tables=tables_out,
            totals=totals_out
        )

        # Save consolidated JSON
        out_json_path = out_json_dir / f"{image_path.stem}_pipeline.json"
        save_json(_to_json(result), out_json_path)

        return result


def _to_json(res: ImageExtractionResult) -> Dict[str, Any]:
    payload = {
        "image": res.image_path,
        "size": {"width": res.width, "height": res.height},
        "tables": [
            {
                "table_index": t.table_index,
                "box": t.box,
                "roles": t.roles,
                "items": t.items,
                "reconciliation": t.reconciliation,
                "crop_path": t.crop_path,
            }
            for t in res.tables
        ]
    }
    if res.totals:
        payload["totals"] = {
            "box": res.totals.box,
            # e.g., {"subtotal": 225.0, "tax_amount": 22.50, "total": 247.5, "amount_due": 247.5}
            "fields": res.totals.fields,
            "raw_lines": res.totals.raw_lines,
            "confidence": res.totals.confidence
        }
    return payload
