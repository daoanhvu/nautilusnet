import os
import tensorflow as tf
from tensorflow.keras.callbacks import ModelCheckpoint
from tensorflow.keras import backend as KB
from PIL import Image
import numpy as np

lambda_rpn_regr = 1.0
lambda_rpn_class = 1.0

lambda_cls_regr = 1.0
lambda_cls_class = 1.0

epsilon = 1e-4

def union(au, bu, area_intersection):
	area_a = (au[2] - au[0]) * (au[3] - au[1])
	area_b = (bu[2] - bu[0]) * (bu[3] - bu[1])
	area_union = area_a + area_b - area_intersection
	return area_union


def intersection(ai, bi):
	x = max(ai[0], bi[0])
	y = max(ai[1], bi[1])
	w = min(ai[2], bi[2]) - x
	h = min(ai[3], bi[3]) - y
	if w < 0 or h < 0:
		return 0
	return w*h


def iou(a, b):
	# a and b should be (x1,y1,x2,y2)

	if a[0] >= a[2] or a[1] >= a[3] or b[0] >= b[2] or b[1] >= b[3]:
		return 0.0

	area_i = intersection(a, b)
	area_u = union(a, b, area_i)

	return float(area_i) / float(area_u + 1e-6)

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
            lambda * sum((binary_crossentropy(isValid*y_pred,y_true))) / N
    '''
    if len(args) == 2:
        y_true = args[0]
        y_pred = args[1]
    else:
        l = len(args)
        y_true = tf.slice(args, begin=(0, 0), size=(l, 1))
        y_pred = tf.slice(args, begin=(0, 1), size=(l, 1))
    
    return lambda_rpn_class * KB.sum(y_true * KB.binary_crossentropy(y_pred, y_true)) / KB.sum(epsilon + y_true)

def build_rpn_model(rpn_input, anchor_count):
    # Step 1: Pass through the output of base model to a 3x3 conv2d layer. Keep padding 'same' to preserve the feature map's size
    output = tf.keras.layers.Conv2D(512, (3, 3), activation="relu", padding="same", name="rpn_conv")(rpn_input)
    # Step 2.1: This layer is used for classification, its a 1x1 filter conv2d
    rpn_cls_output = tf.keras.layers.Conv2D(anchor_count, (1, 1), activation="sigmoid", name="rpn_cls")(output)
    # Step 2.2 This layer is used for bbox regression , its also a 1x1 fiter conv2d
    rpn_reg_output = tf.keras.layers.Conv2D(anchor_count * 4, (1, 1), activation="linear", name="rpn_reg")(output)
    return tf.keras.Model(inputs=rpn_input, outputs=[rpn_reg_output, rpn_cls_output], name='rpn_model')


def train(img_size, channels, anchor_count, training_config):
    # This is the base model, its used for classification, the default activation of this network is softmax
    # and default output classes if 1000
    base_model = tf.keras.applications.VGG16(include_top=False, input_shape=(img_size, img_size, channels))
    feature_extractor = base_model.get_layer("block5_conv3")
    base_model.summary()

    # Build RPN model
    rpn_input = tf.keras.Input(shape=feature_extractor.output.shape, name='rpn_input')
    rpn_model = build_rpn_model(rpn_input, anchor_count=anchor_count)
    rpn_model.summary()

    rpn_model.compile(optimizer=tf.optimizers.Adam(learning_rate=1e-5),
                  loss=[regression_loss, rpn_classification_loss])

    epochs = training_config['epochs']
    batch_size = training_config['batch_size']

    for epoch in range(epochs):
        print('Start epoch %s' % epoch)

        for step, (x_batch_train, y_batch_train) in enumerate(train_ds):
            pass

if __name__=='__main__':
    image_size = 224
    channels = 3
    anchor_count = 9
    training_config = {
        "epochs": 8,
        "batch_size": 4
    }
    train(image_size, channels, anchor_count, training_config)