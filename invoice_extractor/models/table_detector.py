# models/table_detector.py
from typing import List, Dict, Any, Tuple
from PIL import Image
import numpy as np
import torch
from transformers import AutoImageProcessor, DetrImageProcessor, TableTransformerForObjectDetection


class TableRegionDetector:
    """
    Wrapper around Microsoft's Table Transformer (DETR-based) for table region detection.
    Model hub id:
      - "microsoft/table-transformer-detection"
    """

    def __init__(self, model_id: str = "microsoft/table-transformer-detection", device: str | None = None, score_threshold: float = 0.6):
        self.processor = AutoImageProcessor.from_pretrained(model_id)
        # self.processor = DetrImageProcessor()
        self.model = TableTransformerForObjectDetection.from_pretrained(
            model_id)
        if device is None:
            device = "cuda" if torch.cuda.is_available() else "cpu"

        self.device = device
        self.model.to(self.device).eval()
        self.score_threshold = score_threshold

    @torch.no_grad
    def predict(self, image: Image.Image) -> List[Dict[str, Any]]:
        if image.mode != "RGB":
            image = image.convert("RGB")

        encoding = self.processor(
            images=image, return_tensors="pt").to(self.device)
        outputs = self.model(**encoding)

        # Convert outputs to final boxes/scores w.r.t orginal size
        width, height = image.size
        results = self.processor.post_process_object_detection(
            outputs, threshold=self.score_threshold,
            target_sizes=torch.tensor([[height, width]], device=self.device)
        )[0]

        detections: List[Dict[str, Any]] = []
        for score, label_id, box in zip(results["scores"], results["labels"], results["boxes"]):
            detections.append({
                "box": [float(b) for b in box.tolist()],
                "score": float(score.item()),
                "label": self.id_to_label(int(label_id.item()))
            })
        return detections

    @staticmethod
    def id_to_label(class_id: int) -> str:
        # For the detection head the primary class is "table"
        # Some checkpoints include "table rotated", but HF maps to numeric ids.
        # We'll default unknown -> "table".
        mapping = {
            0: "table",          # typical
            1: "table rotated"   # if present in the checkpoint
        }
        return mapping.get(class_id, "table")

    @staticmethod
    def clip_box(box: List[float], w: int, h: int) -> List[int]:
        x0, y0, x1, y1 = box
        x0 = max(0, min(int(np.floor(x0)), w - 1))
        y0 = max(0, min(int(np.floor(y0)), h - 1))
        x1 = max(0, min(int(np.ceil(x1)),  w - 1))
        y1 = max(0, min(int(np.ceil(y1)),  h - 1))
        return [x0, y0, x1, y1]

    def crop_tables(self, image: Image.Image, detections: List[Dict[str, Any]]) -> List[Tuple[Image.Image, Dict[str, Any]]]:
        """
        Returns list of (crop_image, meta) for each detected table.
        """
        W, H = image.size
        outputs = []
        for det in detections:
            x0, y0, x1, y1 = self.clip_box(det["box"], W, H)
            crop = image.crop((x0, y0, x1, y1))
            meta = {"box": [x0, y0, x1, y1],
                    "score": det["score"], "label": det["label"]}
            outputs.append((crop, meta))
        return outputs
