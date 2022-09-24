import json
from mtcnn import MTCNN
import cv2
from detection_utils import detect_and_recognize_faces
from tensorflow.keras.models import load_model
from datetime import datetime
import time
from nqueue.message_sender import MessageSender


SENT_MAP = {}


def send_message(sender: MessageSender, class_index: int, class_name: str):
    previous_check_time = SENT_MAP.get(class_name)
    if previous_check_time is None:
        message = {
            'actionCommand': 'PUNCH_INOUT_CMD',
            'orgId': 1,
            'classIndex': int(class_index),
            'className': class_name,
            'device': 1,
            'datetime': datetime.now().strftime("%Y-%m-%d %H:%M:%S")
        }
        sender.publish_message(message=message)
        SENT_MAP[class_name] = time.time()
    else:
        prev_time = SENT_MAP.get(class_name)
        curr_time = time.time()
        if curr_time - prev_time > 60:
            message = {
                'actionCommand': 'PUNCH_INOUT_CMD',
                'orgId': 1,
                'classIndex': int(class_index),
                'className': class_name,
                'device': 1,
                'datetime': datetime.now().strftime("%Y-%m-%d %H:%M:%S")
            }
            sender.publish_message(message=message)
            SENT_MAP[class_name] = curr_time



def capture_camera():
    project_path = 'D:\\projects\\nautilusnet\\nface'
    class_dictionary = {}
    with open("{}\\data\\classes.json".format(project_path)) as classes_file:
        class_dictionary = json.load(classes_file)
    message_sender = MessageSender(host='localhost', exchange='amq.topic', exchange_type='topic',
        queue_name='nface_messages', topic='nface_detection')
    detector = MTCNN()
    vgg_model = load_model('{}/models/transfer_learning_trained_face_cnn_model_001.h5'.format(project_path))
    vid = cv2.VideoCapture(0)
    font = cv2.FONT_HERSHEY_SIMPLEX
    fontScale = 0.5
    should_detect = True
    while True:
        ret, frame = vid.read()

        if(should_detect):
            if not hasattr(frame, 'shape'):
                continue
            predictions = detect_and_recognize_faces(detector, vgg_model, frame)
            for pred in predictions:
                x_dr, y_dr, w_dr, h_dr = pred['box']
                x_dr, y_dr, w_dr, h_dr = int(x_dr), int(y_dr), int(w_dr), int(h_dr)
                class_name = class_dictionary.get(str(pred['class_index']))
                send_message(sender=message_sender, 
                    class_index=pred['class_index'], class_name=class_name)
                caption = "{} score={}".format(class_name, pred['score'])
                cv2.rectangle(frame, (x_dr,y_dr), (x_dr+w_dr,y_dr+h_dr), (0,255,0), 2)
                frame = cv2.putText(frame, caption, (x_dr, y_dr), font, fontScale, (119,1,133), 1, cv2.LINE_AA)
        
        cv2.imshow('frame', frame)
        # should_detect = not should_detect
        if cv2.waitKey(1) & 0xFF == ord('q'):
            break
    
    vid.release()
    cv2.destroyAllWindows()

if __name__=='__main__':
    capture_camera()