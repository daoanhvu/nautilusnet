# ocr/tesseract_ocr.py
from __future__ import annotations
from typing import List, Dict, Any, Optional
from PIL import Image
import pytesseract
import numpy as np
import cv2


class OCR:
    def __init__(self, lang: str = "eng", psm: int = 6,
                 oem: int = 3,
                 do_preprocess: bool = True,
                 tesseract_cmd: Optional[str] = None):
        if tesseract_cmd:
            pytesseract.pytesseract.tesseract_cmd = tesseract_cmd
            self.lang = lang
            self.psm = psm
            self.oem = oem
            self.do_preprocess = do_preprocess

    def _preprocess(self, image: Image.Image) -> np.ndarray:
        img = np.array(image.convert("RGB"))
        img = cv2.cvtColor(img, cv2.COLOR_RGB2BGR)

        gray = cv2.cvtColor(img, cv2.COLOR_BGR2GRAY)

        th = cv2.adaptiveThreshold(gray, 255, cv2.ADAPTIVE_THRESH_GAUSSIAN_C,
                                   cv2.THRESH_BINARY, 35, 15)

        kernel = np.ones((1, 1), np.uint8)
        th = cv2.dilate(th, kernel, iteractions=1)

        return th

    def run(self, image: Image.Image) -> List[Dict[str, Any]]:
        if self.do_preprocess:
            cv_img = self._preprocess(image)
            pil_ocr = Image.fromarray(cv_img)
        else:
            pil_ocr = image.convert("RGB")

        config = f"--psm {self.psm} --oem {self.oem}"
        data = pytesseract.image_to_data(
            pil_ocr, lang=self.lang, config=config, output_type=pytesseract.Output.DICT
        )
        n = len(data.get("text", []))
        out: List[Dict[str, Any]] = []

        for i in range(n):
            txt = (data["text"][i] or "").strip()
            conf_str = data.get("conf", ["-1"]*n)[i]
            try:
                conf = float(conf_str)
            except Exception:
                conf = -1.0
            
            # Only keep valid words
            if not txt or conf < 0:
                continue

            x, y, w, h = data["left"][i], data["top"][i], data["width"][i], data["height"][i]
            box = [float(x), float(y), float(x+w), float(y+h)]
            out.append({"text": txt, "conf": conf / 100.0, "box": box})
