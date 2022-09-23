import os
from mtcnn import MTCNN
import cv2
import numpy as np
from keras_vggface.vggface import VGGFace
from constants import INPUT_SHAPE, INPUT_IMAGE_TENSOR

MTCNN_THRESHOLD = 0.95

def extend_bounding_box(x, y, w, h, delta):
    dw, dh = w * delta, h * delta
    w1, h1 = w + dw, h + dh
    y1 = y - dh/2.0
    x1 = x - dw/2.0
    if x1 < 0 or y1 < 0:
        return x, y, w, h
    return x1, y1, w1, h1


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
        if score >= MTCNN_THRESHOLD:
            x, y, w, h = detection['box']
            x1, y1, w1, h1 = extend_bounding_box(x, y, w, h, 0.2)
            face_img = img[int(y1):int(y1+h1), int(x1):int(x1+w1)]
            if face_img is not None:
                file_name = "{}/{}_{}.png".format(output_path, file_index, face_count)
                cv2.imwrite(file_name, face_img)
                face_count += 1


def detect_and_recognize_faces(detector: MTCNN, vgg_model: VGGFace, img: cv2.Mat):
    # img = cv2.cvtColor(cv2.imread("ivan.jpg"), cv2.COLOR_BGR2RGB)
    predictions = []
    detections = detector.detect_faces(img)
    for detection in detections:
        score = detection['confidence']
        if score >= MTCNN_THRESHOLD:
            x, y, w, h = detection['box']
            x1, y1, w1, h1 = extend_bounding_box(x, y, w, h, 0.2)
            face_img = img[int(y1):int(y1+h1), int(x1):int(x1+w1)]
            if face_img is not None:
                face_img = cv2.resize(face_img, INPUT_SHAPE)
                face_img = face_img.astype(np.float32)/255.
                face_image = np.reshape(face_img, INPUT_IMAGE_TENSOR)
                prediction = vgg_model.predict(face_image)
                max_class_idx = np.argmax(prediction)
                prediction_result = {
                    'score': prediction,
                    'class_index': max_class_idx,
                    'box': (x1, y1, w1, h1)
                }
                predictions.append(prediction_result)
    return predictions            
