from transformers import LayoutLMv3Processor, LayoutLMv3ForTokenClassification, Trainer, TrainingArguments
from datasets import load_dataset
from PIL import Image
import torch

# Load dataset
dataset = load_dataset("path/to/section_dataset")
processor = LayoutLMv3Processor.from_pretrained("microsoft/layoutlmv3-base", apply_ocr=False)
model = LayoutLMv3ForTokenClassification.from_pretrained("microsoft/layoutlmv3-base", num_labels=4)

# Label mapping
label_map = {"O": 0, "header": 1, "footer": 2, "key_value_pair": 3}

# Preprocess dataset
def preprocess_data(examples):
    images = [Image.open(img).convert("RGB") for img in examples["image"]]
    words = examples["words"]
    boxes = examples["boxes"]
    labels = [[label_map[label] for label in example_labels] for example_labels in examples["labels"]]
    
    # Normalize boxes to [0, 1000]
    normalized_boxes = []
    for img, box_list in zip(images, boxes):
        width, height = img.size
        normalized = [[int(1000 * x / width), int(1000 * y / height),
                       int(1000 * (x + w) / width), int(1000 * (y + h) / height)]
                      for x, y, x2, y2 in box_list]
        normalized_boxes.append(normalized)
    
    encoding = processor(images, words, boxes=normalized_boxes, return_tensors="pt", truncation=True, padding=True)
    encoding["labels"] = torch.tensor(labels, dtype=torch.long)
    return encoding

dataset = dataset.map(preprocess_data, batched=True)

# Training arguments
training_args = TrainingArguments(
    output_dir="./layoutlmv3_finetuned",
    num_train_epochs=10,
    per_device_train_batch_size=4,
    per_device_eval_batch_size=4,
    evaluation_strategy="epoch",
    save_strategy="epoch",
    learning_rate=2e-5,
    load_best_model_at_end=True,
    remove_unused_columns=False
)

# Trainer
trainer = Trainer(
    model=model,
    args=training_args,
    train_dataset=dataset["train"],
    eval_dataset=dataset["validation"]
)

# Train
trainer.train()

# Save model
model.save_pretrained("./layoutlmv3_finetuned")
processor.save_pretrained("./layoutlmv3_finetuned")