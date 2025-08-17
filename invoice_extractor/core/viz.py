# core/viz.py
from typing import List, Dict, Any
from PIL import Image, ImageDraw, ImageFont


def draw_boxes(image: Image.Image, detections: List[Dict], show_label: bool = True):
    img = image.copy().convert("RGB")
    draw = ImageDraw.Draw(img)
    font = None
    try:
        font = ImageFont.load_default()
    except Exception:
        pass

    for d in detections:
        x0, y0, x1, y1 = d["box"]
        draw.rectangle((x0, y0, x1, y1), outline=(255, 0, 0), width=2)
        if show_label:
            txt = f'{d.get("label","table")} {d.get("score",0):.2f}'
            draw.text((x0 + 4, max(0, y0 - 14)), txt,
                      fill=(255, 0, 0), font=font)

    return img
