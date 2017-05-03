import argparse
import os
import csv
import cv2
import numpy as np
import tensorflow as tf
from functools import reduce

from YOLO_TrainingNetwork import YOLO_TrainingNetwork

from plot_utils import *

CLASSES = ["teddy", "ant", "airplane"]
NUM_CLASSES = len(CLASSES)
S = 7
B = 2

def process_image(thenet, sess, img):
    img_resize = cv2.resize(img, (448, 448))
    # for some reason darknet switches red and blue channels...
    # https://github.com/pjreddie/darknet/blob/c6afc7ff1499fbbe64069e1843d7929bd7ae2eaa/src/image.c#L391
    img_resize = cv2.cvtColor(img_resize, cv2.COLOR_BGR2RGB)
    # darknet scales color values from 0 to 1
    # https://github.com/pjreddie/darknet/blob/c6afc7ff1499fbbe64069e1843d7929bd7ae2eaa/src/image.c#L469
    img_resize = (img_resize / 255.0) * 2.0 - 1.0
    input = np.expand_dims(img_resize, axis=0)
    probs, confidences, bboxes = thenet.inference(sess, input)
    # print(bboxes)
    img_out = img.copy()
    return plot_detections_on_im(sess, img_out, probs, confidences, bboxes, CLASSES)

def process_video(video_path):
    frame_detections = []
    cap = cv2.VideoCapture(video_path)
    w = int(cap.get(cv2.cv.CV_CAP_PROP_FRAME_WIDTH))
    h = int(cap.get(cv2.cv.CV_CAP_PROP_FRAME_HEIGHT))
    out = cv2.VideoWriter('data/output.avi', cv2.cv.CV_FOURCC(*'XVID'), 20.0, (w, h))

    i = 0
    frame_window = []
    while cap.isOpened() and i < end_frame:
        ret, frame = cap.read()

        if i >= start_frame:
            print("processing frame " + str(i))
            frame_out, bounding_boxes = self.process_image(frame)
            frame_window.append(bounding_boxes)
            # number of con
            window_size = 3
            if len(frame_window) > 3:
                frame_window.pop(0)
            #if len(frame_window) == 3:
            #    self.group_cropped(frame_window, len(bounding_boxes))
            #frame_detections.append(bounding_boxes)
            #for box in bounding_boxes:
            #    print box.category
            #cv2.imwrite('data/' + str(i) + '.png', frame_out)
            out.write(frame_out)
        i = i + 1
    cap.release()
    out.release()

def group_cropped(frame_window, group_count):
    # concatenate all objects across all frames within the frame window
    x = reduce(list.__add__, frame_window)
    size = frame_window[0][0].img.shape
    centroids = []
    for i in xrange(0, group_count):
        #centroids.append(np.random.randint(255, size=size))
        centroids.append(frame_window[0][i].img)
    c_old = np.zeros(len(x))
    while True:
        c = np.zeros(len(x))
        for i, x_i in enumerate(x):
            c_min = float('inf')
            for j, centroid in enumerate(centroids):
                c_temp = np.square(np.linalg.norm(x_i.img - centroid, 2))
                if c_temp < c_min:
                    c[i] = j
                    c_min = c_temp
        # update centroids
        for j, centroid in enumerate(centroids):
            numerator = np.zeros(size)
            denominator = 0.0
            for i, c_i in enumerate(c):
                if (c_i == j):
                    numerator = numerator + x[i].img
                    denominator = denominator + 1.0
            centroids[j] = numerator / denominator
        # break after convergence
        if (c == c_old).all():
            break
        else:
            c_old = c
    print(c_old)

def main():
    parser = argparse.ArgumentParser()
    parser.add_argument('-w', '--weight_path', help='path to weights/biases for each layer')
    parser.add_argument('-t', '--image_path', help='path to test image')
    parser.add_argument('-c', '--checkpoint_path', help='path to create checkpoint', required=True)
    parser.add_argument('-v', '--video_path', help='path to test video')
    parser.add_argument('-s', '--start', help='video start frame')
    parser.add_argument('-e', '--end', help='video end frame')
    args = parser.parse_args()

    checkpoint_path = os.path.abspath(os.path.expanduser(args.checkpoint_path))
    weight_path = None
    if args.weight_path:
        weight_path = os.path.abspath(os.path.expanduser(args.weight_path))
    yolo = YOLO_TrainingNetwork(use_pretrained_weights = False)
    session = tf.Session()
    session.run(tf.global_variables_initializer())
    saver = tf.train.Saver()
    saver.restore(session, tf.train.latest_checkpoint(checkpoint_path))
    image_path = os.path.abspath(os.path.expanduser(args.image_path))

    img, bounding_boxes = process_image(yolo, session, cv2.imread(image_path))
    cv2.imwrite('data/out.png', img)

if __name__ == "__main__":
    main()
