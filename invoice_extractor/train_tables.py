from transformers import TableTransformerForObjectDetection, DetrImageProcessor, Trainer, TrainingArguments
from datasets import load_dataset
from PIL import Image
import torch

# Load dataset
dataset = load_dataset("path/to/coco_dataset")
processor = DetrImageProcessor.from_pretrained("microsoft/table-transformer-detection")
model = TableTransformerForObjectDetection.from_pretrained("microsoft/table-transformer-detection")

# Preprocess dataset
def preprocess_data(examples):
    images = [Image.open(img).convert("RGB") for img in examples["file_name"]]
    encoded_inputs = processor(images, return_tensors="pt", padding=True)
    encoded_inputs["labels"] = [
        [{"boxes": torch.tensor(ann["bbox"]), "class_labels": torch.tensor([1] * len(ann["bbox"]))}
         for ann in examples["annotations"]]
    ]
    return encoded_inputs

dataset = dataset.map(preprocess_data, batched=True)

# Training arguments
training_args = TrainingArguments(
    output_dir="./table_transformer_finetuned",
    num_train_epochs=10,
    per_device_train_batch_size=2,
    per_device_eval_batch_size=2,
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
model.save_pretrained("./table_transformer_finetuned")
processor.save_pretrained("./table_transformer_finetuned")