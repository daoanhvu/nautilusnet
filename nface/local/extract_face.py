import os
from mtcnn import MTCNN
import cv2

def detect_faces(detector: MTCNN, image_path: str, file_index: int, output_path:str):
    # img = cv2.cvtColor(cv2.imread("ivan.jpg"), cv2.COLOR_BGR2RGB)
    img = cv2.imread(image_path)
    if not hasattr(img, 'shape'):
        print("Error: {}".format(image_path))
        return
    detections = detector.detect_faces(img)
    face_count = 1
    for detection in detections:
        score = detection['confidence']
        if score >= 0.95:
            x, y, w, h = detection['box']
            print("Box: {},{},{},{}".format(x, y, w, h))
            dw = w * 0.2
            dh = h * 0.2
            w1 = w + dw
            h1 = h + dh
            y1 = y - dh/2.0
            x1 = x - dw/2.0
            if x1 < 0 or y1 < 0:
                x1 = x
                w1 = w
                y1 = y
                h1 = h
            face_img = img[int(y1):int(y1+h1), int(x1):int(x1+w1)]
            if face_img is not None:
                file_name = "{}/{}_{}.png".format(output_path, file_index, face_count)
                cv2.imwrite(file_name, face_img)
                face_count += 1

if __name__=='__main__':
    path = r'D:\data\images\new2'
    output_path = r'D:\data\faces'
    included_extensions = ['jpg','jpeg', 'bmp', 'png', 'gif']
    files = [f for f in os.listdir(path) if os.path.isfile(os.path.join(path, f)) 
        and any(f.endswith(ext) for ext in included_extensions)]
    print("Found {} files".format(len(files)))
    detector = MTCNN()
    idx = 1
    for file_path in files:
        file_path = os.path.join(path, file_path)
        print(file_path)
        detect_faces(detector=detector, image_path=file_path, file_index=idx, output_path=output_path)
        idx += 1

