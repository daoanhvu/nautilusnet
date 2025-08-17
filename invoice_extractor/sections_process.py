from transformers import LayoutLMv3Processor, LayoutLMv3ForTokenClassification
from PIL import Image
import pytesseract
import matplotlib.pyplot as plt
from transformers import AutoModelForTokenClassification
import numpy as np
import torch
import cv2

COLORS = [[0.000, 0.447, 0.741], [0.850, 0.325, 0.098], [0.929, 0.694, 0.125],
          [0.494, 0.184, 0.556], [0.466, 0.674, 0.188], [0.301, 0.745, 0.933]]

processor = LayoutLMv3Processor.from_pretrained(
    "microsoft/layoutlmv3-base", apply_ocr=False)
model = LayoutLMv3ForTokenClassification.from_pretrained(
    "impira/layoutlm-invoices")


def plot_result(pil_image, scores, labels, boxes):
    plt.figure(figsize=(16, 10))
    plt.imshow(pil_image)
    ax = plt.gca()
    colors = COLORS * 100

    for score, label, (xmin, ymin, xmax, ymax), c in zip(scores.tolist(), labels.tolist(), boxes.tolist(), colors):
        ax.add_patch(plt.Rectangle((xmin, ymin), xmax - xmin,
                     ymax - ymin, fill=False, color=c, linewidth=2))
        text = f'{model.config.id2label[label]}: {score: 0.2f}'
        ax.text(xmin, ymin, text, fontsize=14, bbox=dict(
            facecolor='yellow', alpha=0.5))

    plt.axis('off')
    plt.show()


def process_image(pil_image):
    # Use pytesseract to extract text and bounding boxes
    ocr_result = pytesseract.image_to_data(
        pil_image, output_type=pytesseract.Output.DICT)

    words = []
    boxes = []

    word_count = len(ocr_result['text'])
    for i in range(word_count):
        if ocr_result['text'][i].strip() != "":
            words.append(ocr_result['text'][i])
            (x, y, w, h) = (ocr_result['left'][i], ocr_result['top']
                            [i], ocr_result['width'][i], ocr_result['height'][i])
            boxes.append([x, y, x + w, y + h])

    # Convert boxes to the format expected by LayoutLMv3
    boxes = np.array(boxes)

    # Should we normalize the boxes to [0, 1000] for LayoutLMv3?
    # Normalize boxes to [0, 1000] for LayoutLMv3
    if boxes.size > 0:
        boxes = boxes.astype(np.float32)
        boxes[:, [0, 2]] = boxes[:, [0, 2]] / pil_image.width * 1000
        boxes[:, [1, 3]] = boxes[:, [1, 3]] / pil_image.height * 1000
        boxes = boxes.clip(0, 1000).astype(int)

    # Prepare inputs for the model
    encoding = processor(pil_image, words, boxes=boxes.tolist(),
                         return_tensors="pt", truncation=True)

    with torch.no_grad():
        outputs = model(**encoding)

    # Get the predicted labels
    logits = outputs.logits
    # Some people use `logits.argmax(-1)` instead of `torch.argmax(logits, dim=2)`
    predictions = logits.argmax(-1)[0].tolist()  # torch.argmax(logits, dim=2)

    labels = [model.config.id2label[pred] for pred in predictions]
    # Print the results
    for word, box, label in zip(words, boxes.tolist(), labels):
        print(f"Word: {word}, Box: {box}, Label: {label}")

    # Plot the boxes on the image
    # plot_result(pil_image=pil_image, scores=predictions,
    #             labels=labels, boxes=boxes)
    plot_boxes(pil_image, predictions, labels, boxes)


def plot_boxes(pil_image, scores, labels, boxes):
    # Convert PIL image to OpenCV format
    img = np.array(pil_image)
    if img.shape[2] == 4:
        img = cv2.cvtColor(img, cv2.COLOR_RGBA2BGR)
    else:
        img = cv2.cvtColor(img, cv2.COLOR_RGB2BGR)

    # Assign a color for each unique label
    unique_labels = list(set(labels))
    color_map = {label: tuple(np.random.randint(0, 255, 3).tolist())
                 for label in unique_labels}

    for box, label in zip(boxes, labels):
        x0, y0, x1, y1 = [int(coord / 1000 * img.shape[1]) if i % 2 == 0 else int(
            coord / 1000 * img.shape[0]) for i, coord in enumerate(box)]
        color = color_map[label]
        cv2.rectangle(img, (x0, y0), (x1, y1), color, 2)
        cv2.putText(img, label, (x0, y0 - 5),
                    cv2.FONT_HERSHEY_SIMPLEX, 0.5, color, 1, cv2.LINE_AA)

    # Show the image in a window
    cv2.imshow('Labeled Boxes', img)
    cv2.waitKey(0)
    cv2.destroyAllWindows()


if __name__ == "__main__":
    # Example usage
    image_path = "/Users/vudao/projects/data/invoices/invoice-001.png"
    pil_image = Image.open(image_path).convert("RGB")

    process_image(pil_image)
    print("Image processed successfully.")
