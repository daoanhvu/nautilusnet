import os
import torch
import cv2
from facenet_pytorch import MTCNN, InceptionResnetV1
from tqdm import tqdm
from utils import detect_box, load_data
from types import MethodType

device = torch.device('cuda' if torch.cuda.is_available() else 'cpu')

def load_embeddings(mtcnn, model, data_dir, data_info):
    embeddings = []
    embedding_folder = os.path.join(data_dir, 'for_embeddings')

    for i, class_name in enumerate(data_info['class_names']):
        class_path = os.path.join(embedding_folder, class_name)
        images = [os.path.join(class_path, img) for img in os.listdir(class_path)]
        img = cv2.imread(images[0])
        boxes, cropped_images = mtcnn.detect_box(img)
        if cropped_images is not None:
            for box, cropped in zip(boxes, cropped_images):
                img_embedding = model(torch.Tensor(cropped.unsqueeze(0)).to(device))
                embeddings.append({
                    'class_name': class_name,
                    'embedding': img_embedding
                })
    return embeddings
                
        

def capture_camera(mtcnn, resnet, embeddings):
    vid = cv2.VideoCapture(0)
    font = cv2.FONT_HERSHEY_SIMPLEX
    fontScale = 0.5
    
    while vid.grab():
        _, frame = vid.retrieve()
        batch_boxes, cropped_images = mtcnn.detect_box(frame)
        if cropped_images is not None:
            for box, cropped in zip(batch_boxes, cropped_images):
                x, y, x2, y2 = [int(x) for x in box]
                img_embedding = resnet(torch.Tensor(cropped.unsqueeze(0)).to(device))
                detect_dict = {}
                for v in embeddings:
                    detect_dict[v["class_name"]] = (v["embedding"] - img_embedding).norm().item()
                min_key = min(detect_dict, key=detect_dict.get)
                
                if detect_dict[min_key] >= 0.5:
                    min_key = 'Undetected ({})'.format(detect_dict[min_key])
                
                cv2.rectangle(frame, (x, y), (x2, y2), (0, 0, 255), 2)
                cv2.putText(frame, min_key, (x + 5, y + 10), font, fontScale, (255, 255, 255), 1)
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

    mtcnn = MTCNN(image_size=224, keep_all=True, device=device, thresholds=[0.4, 0.5, 0.5], min_face_size=60)
    mtcnn.detect_box = MethodType(detect_box, mtcnn)

    # Load embeddings
    embeddings = load_embeddings(mtcnn=mtcnn, model=resnet, data_dir=data_dir, data_info=data_info)

    capture_camera(mtcnn=mtcnn, resnet=resnet, embeddings=embeddings)

if __name__ == "__main__":
    main()