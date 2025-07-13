from transformers import TableTransformerForObjectDetection, DetrImageProcessor, LayoutLMv3Processor, LayoutLMv3ForTokenClassification
from PIL import Image
import pytesseract
import cv2
import numpy as np
import torch
import json

# Load fine-tuned models and processors
table_processor = DetrImageProcessor.from_pretrained("./table_transformer_finetuned")
table_model = TableTransformerForObjectDetection.from_pretrained("./table_transformer_finetuned")
section_processor = LayoutLMv3Processor.from_pretrained("./layoutlmv3_finetuned", apply_ocr=False)
section_model = LayoutLMv3ForTokenClassification.from_pretrained("./layoutlmv3_finetuned")

# Load invoice image
image = Image.open("invoice.jpg").convert("RGB")
image_np = np.array(image)  # For OpenCV processing
image_width, image_height = image.size

# Step 1: Table detection and splitting
def detect_and_split_tables(image, image_np):
    inputs = table_processor(images=image, return_tensors="pt")
    with torch.no_grad():
        outputs = table_model(**inputs)
    
    target_sizes = torch.tensor([image.size[::-1]])  # [height, width]
    results = table_processor.post_process_object_detection(outputs, target_sizes=target_sizes, threshold=0.9)[0]
    
    table_boxes = []
    for score, label, box in zip(results["scores"], results["labels"], results["boxes"]):
        if table_model.config.id2label[label.item()] == "table":
            box = [round(coord.item()) for coord in box]  # [x_min, y_min, x_max, y_max]
            table_boxes.append({"score": score.item(), "box": box})
    
    def split_merged_tables(table_box, gap_threshold=50):
        x_min, y_min, x_max, y_max = table_box["box"]
        table_region = image_np[y_min:y_max, x_min:x_max]
        ocr_result = pytesseract.image_to_data(table_region, output_type=pytesseract.Output.DICT)
        words = [w for w in ocr_result["text"] if w.strip() != ""]
        boxes = [[left + x_min, top + y_min, left + x_min + width, top + y_min + height]
                 for left, top, width, height in zip(
                     ocr_result["left"], ocr_result["top"], ocr_result["width"], ocr_result["height"])
                 if ocr_result["text"][ocr_result["text"].index(w)].strip() != ""]
        
        x_coords = [(box[0] + box[2]) / 2 for box in boxes]
        if not x_coords:
            return [table_box]
        
        x_coords_sorted = sorted(x_coords)
        gaps = [(x_coords_sorted[i+1] - x_coords_sorted[i]) for i in range(len(x_coords_sorted)-1)]
        split_points = [(x_coords_sorted[i] + x_coords_sorted[i+1]) / 2
                        for i, gap in enumerate(gaps) if gap > gap_threshold]
        
        if not split_points:
            return [table_box]
        
        split_boxes = []
        prev_x = x_min
        for split_x in split_points + [x_max]:
            split_boxes.append({"score": table_box["score"], "box": [round(prev_x), y_min, round(split_x), y_max]})
            prev_x = split_x
        return split_boxes
    
    all_table_boxes = []
    for table_box in table_boxes:
        all_table_boxes.extend(split_merged_tables(table_box))
    
    return all_table_boxes

# Step 2: OCR for all text
def extract_ocr_data(image):
    ocr_result = pytesseract.image_to_data(image, output_type=pytesseract.Output.DICT, config='--psm 6')
    words = [word for word in ocr_result["text"] if word.strip() != ""]
    boxes = [[left, top, left + width, top + height]
             for left, top, width, height in zip(
                 ocr_result["left"], ocr_result["top"], ocr_result["width"], ocr_result["height"])
             if ocr_result["text"][ocr_result["text"].index(word)].strip() != ""]
    return words, boxes

# Step 3: Filter non-table text
def filter_non_table_text(words, boxes, table_boxes):
    non_table_words = []
    non_table_boxes = []
    for word, box in zip(words, boxes):
        is_in_table = any(x_min <= box[0] <= x_max and y_min <= box[1] <= y_max
                          for x_min, y_min, x_max, y_max in [tb["box"] for tb in table_boxes])
        if not is_in_table:
            non_table_words.append(word)
            non_table_boxes.append(box)
    return non_table_words, non_table_boxes

# Step 4: Classify non-table sections with LayoutLMv3
def classify_sections(image, words, boxes):
    normalized_boxes = [[int(1000 * left / image_width), int(1000 * top / image_height),
                        int(1000 * (left + width) / image_width), int(1000 * (top + height) / image_height)]
                       for left, top, width, height in boxes]
    
    encoding = section_processor(image, words, boxes=normalized_boxes, return_tensors="pt", truncation=True, padding=True)
    with torch.no_grad():
        outputs = section_model(**encoding)
    
    predictions = outputs.logits.argmax(-1)[0].tolist()
    labels = ["O", "header", "footer", "key_value_pair"]
    classified_sections = {"header": [], "footer": [], "key_value_pair": [], "other": []}
    
    for i, (word, box, pred) in enumerate(zip(words, boxes, predictions)):
        section = labels[pred] if pred < len(labels) else "other"
        classified_sections[section].append({"text": word, "box": box})
    
    def group_section_boxes(section_items):
        if not section_items:
            return None
        x_min = min(item["box"][0] for item in section_items)
        y_min = min(item["box"][1] for item in section_items)
        x_max = max(item["box"][2] for item in section_items)
        y_max = max(item["box"][3] for item in section_items)
        return [x_min, y_min, x_max, y_max]
    
    section_boxes = {section: group_section_boxes(items) for section, items in classified_sections.items() if items}
    return classified_sections, section_boxes

# Step 5: Run pipeline
table_boxes = detect_and_split_tables(image, image_np)
words, boxes = extract_ocr_data(image)
non_table_words, non_table_boxes = filter_non_table_text(words, boxes, table_boxes)
classified_sections, section_boxes = classify_sections(image, non_table_words, non_table_boxes)

# Step 6: Combine and output results
output = {
    "tables": [
        {"table_id": i + 1, "score": table["score"], "box": table["box"]}
        for i, table in enumerate(table_boxes)
    ],
    "sections": {
        section: [
            {"text": item["text"], "box": item["box"]}
            for item in items
        ] for section, items in classified_sections.items() if items
    },
    "section_boxes": section_boxes
}

# Save output to JSON
with open("invoice_output.json", "w") as f:
    json.dump(output, f, indent=2)

# Step 7: Visualize results
img = cv2.imread("invoice.jpg")
colors = {
    "table": (0, 255, 0),
    "header": (0, 0, 255),
    "footer": (255, 0, 0),
    "key_value_pair": (255, 255, 0),
    "other": (128, 128, 128)
}

for i, table in enumerate(table_boxes):
    x_min, y_min, x_max, y_max = table["box"]
    cv2.rectangle(img, (x_min, y_min), (x_max, y_max), colors["table"], 2)
    cv2.putText(img, f"Table {i+1} (score: {table['score']:.2f})", (x_min, y_min - 10),
                cv2.FONT_HERSHEY_SIMPLEX, 0.9, colors["table"], 2)

for section, box in section_boxes.items():
    if box:
        x_min, y_min, x_max, y_max = box
        color = colors.get(section, (255, 255, 255))
        cv2.rectangle(img, (x_min, y_min), (x_max, y_max), color, 2)
        cv2.putText(img, section, (x_min, y_min - 10), cv2.FONT_HERSHEY_SIMPLEX, 0.9, color, 2)

cv2.imwrite("combined_invoice.jpg", img)

# Print summary
print("Detected tables:", len(table_boxes))
for table in table_boxes:
    print(f"Table: {table['box']}, Score: {table['score']:.2f}")
print("\nDetected sections:")
for section, items in classified_sections.items():
    if items:
        print(f"{section}: {len(items)} items, Bounding Box: {section_boxes[section]}")