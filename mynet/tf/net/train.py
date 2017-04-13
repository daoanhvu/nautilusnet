import sys
import os
import tensorflow.contrib.slim as slim
import tensorflow as tf
import cv2

from yolo import YOLO

def read_data(datafile):
    """Read test data"""
    f = open(datafile, "r")
    input_files = f.readlines()
    for i in input_files:
        fname = i.replace("\n", "")
        print(fname)
    #img = cv2.imread('', 0)
    #Read list of file

def loss():
    S = 7
    B = 2
    C = 5
    SS = S * S
    for i in range(SS):
        for b in range(B):
            print (i)

def main():
    sess = tf.InteractiveSession()
    input_data = tf.placeholder(tf.float32, shape=[None, 200704, 3])


    yolo = YOLO()

    print(yolo)


main()
