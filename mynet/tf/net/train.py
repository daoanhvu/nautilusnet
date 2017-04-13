import sys
import os
import tensorflow.contrib.slim as slim
import tensorflow as tf
import cv2

from tools.process_config import process_configure
from optparse import OptionParser
from yolo import YOLO


def main():
    parser = OptionParser()
    parser.add_option("-c", "--conf", dest="configure", help="configure filename")
    (options, args) = parser.parse_args()
    if options.configure:
        config_file = str(options.configure)
        print(config_file)
    else:
        print("Please feed argument configure filename!")
        exit(0)

    process_configure(config_file)

    #sess = tf.InteractiveSession()
    #input_data = tf.placeholder(tf.float32, shape=[None, 200704, 3])
    #yolo = YOLO()
    #print(yolo)

main()
