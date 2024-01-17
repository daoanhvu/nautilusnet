import os
import torch
import cv2
from facenet_pytorch import MTCNN, InceptionResnetV1
from tqdm import tqdm
from utils import detect_box, load_data
from types import MethodType

device = torch.device('cuda' if torch.cuda.is_available() else 'cpu')

def capture_camera(mtcnn, resnet, classes, prob_fn):
    vid = cv2.VideoCapture(0)
    font = cv2.FONT_HERSHEY_SIMPLEX
    fontScale = 0.5
    
    while vid.grab():
        _, frame = vid.retrieve()
        batch_boxes, cropped_images = mtcnn.detect_box(frame)
        if cropped_images is not None:
            for box, cropped in zip(batch_boxes, cropped_images):
                x, y, x2, y2 = [int(x) for x in box]
                outputs = resnet(torch.Tensor(cropped.unsqueeze(0)).to(device))
                probs = prob_fn(outputs)
                _, predicted = torch.max(outputs, 1)
                caption = '{} ({})'.format(classes[predicted[0]], probs[0][0])
                cv2.rectangle(frame, (x, y), (x2, y2), (0, 0, 255), 2)
                cv2.putText(frame, caption, (x + 5, y + 10), font, fontScale, (255, 255, 255), 1)
        cv2.imshow('frame', frame)
        if cv2.waitKey(1) & 0xFF == ord('q'):
            cv2.destroyAllWindows()
            break

def main():
    data_dir = './dataset'
    model_path = "../models/resnet_vggface.pt"

    data_info = load_data(data_dir=data_dir)

    resnet = InceptionResnetV1(pretrained='vggface2', classify=True, device=device, num_classes=len(data_info["class_names"]))
    resnet.to(device=device)
    resnet.load_state_dict(torch.load(model_path))
    resnet.eval()

    mtcnn = MTCNN(image_size=224, keep_all=True, device=device, thresholds=[0.4, 0.5, 0.5], min_face_size=32)
    mtcnn.detect_box = MethodType(detect_box, mtcnn)

    prob_fn = torch.nn.Softmax(dim=1)

    capture_camera(mtcnn=mtcnn, resnet=resnet, classes=data_info['class_names'], prob_fn=prob_fn)

if __name__ == "__main__":
    main()