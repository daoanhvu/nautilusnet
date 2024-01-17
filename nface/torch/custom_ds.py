import os
from torch.utils.data import Dataset, DataLoader
from torchvision import transforms
from facenet_pytorch import MTCNN, InceptionResnetV1

class CustomDataset(Dataset):
    def __init__(self, data_dir, transform=None, mtcnn=None):
        self.data_dir = data_dir
        self.transform = transform
        self.mtcnn = mtcnn
        self.image_paths, self.labels = self.load_data()

    def load_data(self):
        # Assuming your data is organized in subdirectories, each representing a class
        classes = os.listdir(self.data_dir)
        image_paths = []
        labels = []

        for i, class_name in enumerate(classes):
            class_path = os.path.join(self.data_dir, class_name)
            images = [os.path.join(class_path, img) for img in os.listdir(class_path)]
            image_paths.extend(images)
            labels.extend([i] * len(images))

        return image_paths, labels

    def __len__(self):
        return len(self.image_paths)

    def __getitem__(self, idx):
        img_path = self.image_paths[idx]
        label = self.labels[idx]

        # Load the original image
        original_image = Image.open(img_path).convert('RGB')

        # Apply MTCNN for face detection
        if self.mtcnn is not None:
            boxes, _ = self.mtcnn.detect(original_image)

            # If no face is detected, return the original image with label
            if boxes is None:
                if self.transform is not None:
                    return self.transform(original_image), label
                else:
                    return original_image, label

            # Extract the first detected face
            x1, y1, x2, y2 = boxes[0].astype(int)
            face_image = original_image.crop((x1, y1, x2, y2))

            # Apply the transformation to the face region
            if self.transform is not None:
                face_image = self.transform(face_image)

            return face_image, label

        # If MTCNN is not provided, return the original image with label
        if self.transform is not None:
            return self.transform(original_image), label
        else:
            return original_image, label

# Set up MTCNN and other transformations
mtcnn = MTCNN(keep_all=True)
val_trans = transforms.Compose([
    transforms.Resize(299),
    transforms.CenterCrop(299),
    transforms.ToTensor(),
    transforms.Normalize([0.485, 0.456, 0.406], [0.229, 0.224, 0.225])
])

# Create an instance of the custom dataset
val_dataset = CustomDataset(data_dir='path/to/your/data', transform=val_trans, mtcnn=mtcnn)

# Create a DataLoader
val_dataloader = DataLoader(val_dataset, batch_size=32, shuffle=True)
