import os
import torch
import cv2
import copy
import time
from utils import detect_box, load_data
from facenet_pytorch import MTCNN, InceptionResnetV1
from types import MethodType
import argparse

device = torch.device('cuda' if torch.cuda.is_available() else 'cpu')

def test_recognition(image_path, prob_fn):
    data_dir = './dataset'
    model_path = "../models/resnet_vggface.pt"

    data_info = load_data(data_dir=data_dir)
    classes = data_info['class_names']

    resnet = InceptionResnetV1(pretrained='vggface2', classify=True, device=device, num_classes=len(data_info["class_names"]))
    resnet.to(device=device)
    resnet.load_state_dict(torch.load(model_path))
    resnet.eval()

    mtcnn = MTCNN(image_size=224, keep_all=True, device=device, thresholds=[0.4, 0.5, 0.5], min_face_size=32)
    mtcnn.detect_box = MethodType(detect_box, mtcnn)

    font = cv2.FONT_HERSHEY_SIMPLEX
    fontScale = 0.5

    image = cv2.imread(image_path)
    batch_boxes, cropped_images = mtcnn.detect_box(image)
    if cropped_images is not None:
        for box, cropped in zip(batch_boxes, cropped_images):
            x, y, x2, y2 = [int(round(max(x,0))) for x in box]
            outputs = resnet(torch.Tensor(cropped.unsqueeze(0)).to(device))
            _, predicted = torch.max(outputs, 1)
            probs = prob_fn(outputs)
            print('outputs: {} predited: {} {}'.format(outputs, predicted, probs[0][0]))
            caption = classes[predicted[0]]
            cv2.rectangle(image, (x, y), (x2, y2), (0, 0, 255), 2)
            cv2.putText(image, caption, (x + 5, y + 10), font, fontScale, (255, 255, 255), 1)

        cv2.imshow('frame', image)
        cv2.waitKey(0)
        cv2.destroyAllWindows()
        cv2.waitKey(1)

if __name__ == '__main__':
    parser = argparse.ArgumentParser()
    parser.add_argument('--image', type=str, required=False, help='Image to test')

    prob_fn = torch.nn.Softmax(dim=1)
    
    args = parser.parse_args()
    test_recognition(args.image, prob_fn)
