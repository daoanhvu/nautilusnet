import sys
import os
import tensorflow.contrib.slim as slim
import tensorflow as tf
import cv2

from tools.process_config import process_configure
from optparse import OptionParser
from net.yolo import YOLO


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

    common_params, dataset_params, net_params, solver_params = process_configure(config_file)
    print(common_params)
    #sess = tf.InteractiveSession()
    #input_data = tf.placeholder(tf.float32, shape=[None, 200704, 3])
    #yolo = YOLO()
    #print(yolo)

if __name__=="__main__":
    main()
