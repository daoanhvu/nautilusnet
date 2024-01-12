import os
import torch
import cv2
from facenet_pytorch import MTCNN, InceptionResnetV1
from tqdm import tqdm
from types import MethodType

def detect_box(self, img, save_path=None):
    # Detect faces
    batch_boxes, batch_probs, batch_points = self.detect(img, landmarks=True)
    # Select faces
    if not self.keep_all:
        batch_boxes, batch_probs, batch_points = self.select_boxes(
            batch_boxes, batch_probs, batch_points, img, method=self.selection_method
        )
    # Extract faces
    faces = self.extract(img, batch_boxes, save_path)
    return batch_boxes, faces

def capture_camera(mtcnn, resnet):
    vid = cv2.VideoCapture(0)
    font = cv2.FONT_HERSHEY_SIMPLEX
    fontScale = 0.5
    
    while vid.grab():
        _, frame = vid.retrieve()
        batch_boxes, cropped_images = mtcnn.detect_box(frame)
        if cropped_images is not None:
            for box, cropped in zip(batch_boxes, cropped_images):
                x, y, x2, y2 = [int(x) for x in box]
                # img_embedding = resnet(torch.Tensor(cropped.unsqueeze(0)))
                # detect_dict = {}
                # for k, v in all_people_faces.items():
                #     detect_dict[k] = (v - img_embedding).norm().item()
                # min_key = min(detect_dict, key=detect_dict.get)
                min_key = 'Detected'

                # if detect_dict[min_key] >= thres:
                #     min_key = 'Undetected'
                
                cv2.rectangle(frame, (x, y), (x2, y2), (0, 0, 255), 2)
                cv2.putText(frame, min_key, (x + 5, y + 10), font, 0.5, (255, 255, 255), 1)
        cv2.imshow('frame', frame)
        if cv2.waitKey(1) & 0xFF == ord('q'):
            cv2.destroyAllWindows()
            break

def main():
    resnet = InceptionResnetV1(pretrained='vggface2').eval()
    mtcnn = MTCNN(image_size=224, keep_all=True, thresholds=[0.4, 0.5, 0.5], min_face_size=60)
    mtcnn.detect_box = MethodType(detect_box, mtcnn)
    capture_camera(mtcnn=mtcnn, resnet=resnet)

if __name__ == "__main__":
    main()