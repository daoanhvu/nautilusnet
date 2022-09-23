import json
import os
from mtcnn import MTCNN
import cv2
import numpy as np
from keras_vggface.vggface import VGGFace
from keras_vggface import utils
from tensorflow.keras.models import load_model
from constants import INPUT_SHAPE, INPUT_IMAGE_TENSOR
from detection_utils import extend_bounding_box, MTCNN_THRESHOLD

def detect_faces(detector: MTCNN, image_path: str, vgg_model: VGGFace, class_dictionary):
    # img = cv2.cvtColor(cv2.imread("ivan.jpg"), cv2.COLOR_BGR2RGB)
    font = cv2.FONT_HERSHEY_SIMPLEX
    fontScale = 0.5
    img = cv2.imread(image_path)
    if not hasattr(img, 'shape'):
        print("Error: {}".format(image_path))
        return
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
                    'score': prediction[0][max_class_idx],
                    'class_index': max_class_idx,
                    'box': (x1, y1, w1, h1)
                }
                predictions.append(prediction_result)

    for pred in predictions:
        x_dr, y_dr, w_dr, h_dr = pred['box']
        x_dr, y_dr, w_dr, h_dr = int(x_dr), int(y_dr), int(w_dr), int(h_dr)
        class_name = class_dictionary.get(str(pred['class_index']))
        # print(pred['score'][0][pred['class_index']])
        caption = "{} score={}".format(class_name, pred['score'])
        cv2.rectangle(img, (x_dr,y_dr), (x_dr+w_dr,y_dr+h_dr), (0,255,0), 2)
        img = cv2.putText(img, caption, (x_dr, y_dr), font, fontScale, (119,1,133), 1, cv2.LINE_AA)
    while True:
        cv2.imshow("Result", img)
        if cv2.waitKey(1) & 0xFF == ord('q'):
            return


if __name__=='__main__':
    project_path = 'D:\\projects\\nautilusnet\\nface'
    image_path = 'D:\\data\\images\\185329_10150271303542906_7648119_n.jpg'
    class_dictionary = {}
    with open("{}\\data\\classes.json".format(project_path)) as classes_file:
        class_dictionary = json.load(classes_file)
    detector = MTCNN()
    vgg_model = load_model('{}/models/transfer_learning_trained_face_cnn_model_001.h5'.format(project_path))
    detect_faces(detector, image_path, vgg_model, class_dictionary)
