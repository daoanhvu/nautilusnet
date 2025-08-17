# models/table_detector.py
from typing import List, Dict, Any, Tuple
from PIL import Image
import numpy as np
import torch
from transformers import AutoImageProcessor, TableTransformerForObjectDetection


class TableRegionDetector:
    """
    """

    def __init__(self, model_id: str = "microsoft/table-transformer-detection",
                 device: str | None = None, score_threshold: float = 0.6):
        self.processor = AutoImageProcessor.from_pretrained(model_id)
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
        
        width, height = image.size
        encoding = self.processor(images=image, retun_tensors="pt").to(self.device)
        outputs = self.model(**encoding)

        # Convert outputs to final boxes/scores w.r.t orginal size
        results = self.processor.post_process_object_detection(
            outputs, threshold=self.score_threshold,
            target_sizes=torch.tensor([[height, width]], device=self.device)[0]
        )
