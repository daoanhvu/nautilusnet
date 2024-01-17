import os
import torch
import cv2
from torchvision import datasets, transforms
from torch.utils.data import DataLoader
from facenet_pytorch import MTCNN, InceptionResnetV1
from types import MethodType
from utils import detect_box

device = torch.device('cuda' if torch.cuda.is_available() else 'cpu')


if __name__=='__main__':
    # Load the dataset
    data_dir = './dataset/'
    batch_size = 4

    classes = os.listdir(os.path.join(data_dir, 'train'))
    
    # Set up MTCNN for face detection
    mtcnn = MTCNN(image_size=224, keep_all=True, device=device, thresholds=[0.4, 0.5, 0.5], min_face_size=32)
    # mtcnn.detect_box = MethodType(detect_box, mtcnn)

    # image = cv2.imread("D:\\projects\\nautilusnet\\nface\\torch\\dataset\\train\\AnPhuc\\anphuc1.PNG")
    # print(image)
    # batch_boxes, batch_probs = mtcnn.detect(image)
    # print(batch_boxes)
    # print(batch_probs)

    for cls in classes:
        class_path = os.path.join('./dataset/train_preprocess/', cls)
        image_paths = [os.path.join(class_path, f) for f in os.listdir(class_path) if os.path.isfile(os.path.join(class_path, f)) ]
        processed_path = os.path.join('./dataset/train/', cls)
        isExist = os.path.exists(processed_path)
        if not isExist:
            os.makedirs(processed_path)

        idx = 1
        for image in image_paths:
            print('Detecting {}'.format(image))
            image_data = cv2.imread(image)
            batch_boxes, batch_probs = mtcnn.detect(image_data)
            if batch_boxes is not None:
                for box, prob in zip(batch_boxes, batch_probs):
                    print('Box {} prob: {}'.format(box, prob))
                    x, y, x1, y1 = [int(round(max(x, 0))) for x in box]
                    processed_image_data = image_data[y:y1, x:x1]
                    if processed_image_data is not None:
                        cv2.imwrite('{}/{}{}.png'.format(processed_path, cls, idx), processed_image_data)
                        idx = idx + 1
            else:
                print('MTCNN could NOT detect this image')
        print('=====================')
