import tensorflow as tf
from tensorflow.keras.layers import ( 
    Add, 
    Input, 
    Lambda,
    Conv2D, 
    LeakyReLU, 
    ZeroPadding2D, 
    UpSampling2D,
    MaxPool2D )
import numpy as np
import mynet.yltf.simple_net as sn

def YoloV3(size=None, channels=3, classes=80, training=False):
	simpleNet = sn.SimpleNet()
	x = inputs = Input([size, size, channels])
	x_36, x_61, x = simpleNet.build_darknet('yolo_darknet')(x)
	x = simpleNet.yolo_conv()(x)
	return tf.keras.Model(inputs, outputs, name='yolov3')