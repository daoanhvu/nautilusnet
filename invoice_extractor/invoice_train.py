from transformers import DetrImageProcessor
from transformers import TableTransformerForObjectDetection
from PIL import Image
import torch
import matplotlib.pyplot as plt
import os
import pandas

COLORS = [[0.000, 0.447, 0.741], [0.850, 0.325, 0.098], [0.929, 0.694, 0.125],
          [0.494, 0.184, 0.556], [0.466, 0.674, 0.188], [0.301, 0.745, 0.933]]

# Load model
model = TableTransformerForObjectDetection.from_pretrained(
    "microsoft/table-transformer-detection")


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


def table_extraction(image):
    # Prepare image
    processor = DetrImageProcessor()
    encoding = processor(images=image, return_tensors="pt")
    with torch.no_grad():
        outputs = model(**encoding)

    # Get table bounding boxes
    width, height = image.size
    results = processor.post_process_object_detection(
        outputs=outputs, threshold=0.85, target_sizes=[(height, width)])
    print("Number of bounding box: ", len(results))
    result = results[0]
    plot_result(image, result['scores'], result['labels'], result['boxes'])
    return result['boxes']


if __name__ == '__main__':
    image = Image.open(
        "/Users/vudao/projects/data/invoices/invoice-001.png").convert("RGB")
    table_extraction(image=image)
