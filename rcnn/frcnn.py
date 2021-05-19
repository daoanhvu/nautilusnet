import os
import tensorflow as tf
import tensorflow_datasets as tfds
from tensorflow.keras.callbacks import ModelCheckpoint
from tensorflow.keras import backend as KB
from PIL import Image
import numpy as np

lambda_rpn_regr = 1.0
lambda_rpn_class = 1.0

lambda_cls_regr = 1.0
lambda_cls_class = 1.0

epsilon = 1e-4

def regression_loss(*args):
    '''
        Input:
            *args can be (y_true, y_pred) OR ( (y_true, y_pred), ... ) for batch processing
        Loss function for RPN regression
        Output:
            Smooth L1 loss function
                    0.5 * x^2 (if abs(x) < 1)
                    abs(x) - 0.5 (otherwise)
    '''
    if len(args) == 2:
        y_true = args[0]
        y_pred = args[1]
    else:
        l = len(args)
        y_true = tf.slice(args, begin=(0, 0), size=(l, 1))
        y_pred = tf.slice(args, begin=(0, 1), size=(l, 1))
    
    x = y_true - y_pred
    x_abs = KB.abs(x)
    x_bool = KB.cast(KB.less_equal(x_abs, 1.0), tf.float32)
    return lambda_rpn_regr * KB.sum(y_true * (x_bool * (0.5 * x * x) + (1 - x_bool) * (x_abs - 0.5))) / KB.sum(epsilon + y_true)

def rpn_classification_loss(*args):
    '''
        Input:
            *args can be (y_true, y_pred) OR ( (y_true, y_pred), ... ) for batch processing
        Loss function for RPN regression
        Output:
    '''
    if len(args) == 2:
        y_true = args[0]
        y_pred = args[1]
    else:
        l = len(args)
        y_true = tf.slice(args, begin=(0, 0), size=(l, 1))
        y_pred = tf.slice(args, begin=(0, 1), size=(l, 1))
    pass

def build_rpn_model(image_input, feature_extractor, anchor_count):
    # Step 1: Pass through the output of base model to a 3x3 conv2d layer. Keep padding 'same' to preserve the feature map's size
    output = tf.keras.layers.Conv2D(512, (3, 3), activation="relu", padding="same", name="rpn_conv")(feature_extractor.output)
    # Step 2.1: This layer is used for classification, its a 1x1 filter conv2d
    rpn_cls_output = tf.keras.layers.Conv2D(anchor_count, (1, 1), activation="sigmoid", name="rpn_cls")(output)
    # Step 2.2 This layer is used for bbox regression , its also a 1x1 fiter conv2d
    rpn_reg_output = tf.keras.layers.Conv2D(anchor_count * 4, (1, 1), activation="linear", name="rpn_reg")(output)
    return tf.keras.Model(inputs=image_input, outputs=[rpn_reg_output, rpn_cls_output], name='rpn_model')


def train(img_size, channels, anchor_count):
    # This is the base model, its used for classification, the default activation of this network is softmax
    # and default output classes if 1000
    base_model = tf.keras.applications.VGG16(include_top=False, input_shape=(img_size, img_size, channels))
    feature_extractor = base_model.get_layer("block5_conv3")
    base_model.summary()

    # Build RPN model
    rpn_model = build_rpn_model(base_model.input, feature_extractor, anchor_count=anchor_count)
    rpn_model.summary()

    # rpn_model.compile(optimizer=tf.optimizers.Adam(learning_rate=1e-5),
    #               loss=[train_utils.reg_loss, train_utils.rpn_cls_loss])

if __name__=='__main__':
    image_size = 224
    channels = 3
    anchor_count = 9
    train(image_size, channels, anchor_count)