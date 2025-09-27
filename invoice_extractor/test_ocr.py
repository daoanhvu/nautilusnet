# ocr/test_ocr.py
import pytesseract
from ocr.tesseract_ocr import OCR
from PIL import Image
from typing import List, Dict, Any
from scripts.rowcol_infer import cluster_rows


def main(image_path: str):
    ocr = OCR()
    image = Image.open(image_path).convert('RGB')
    tokens = ocr.run(image)

    if not tokens:
        return

    # print(tokens)
    row_token_idxs = cluster_rows(tokens=tokens, distance_px=18.0)
    print(row_token_idxs)


if __name__ == '__main__':
    import sys
    if len(sys.argv) < 2:
        print("Usage: python test_ocr.py <image_path>")
    else:
        main(sys.argv[1])
