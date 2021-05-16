import tensorflow as tf
import numpy as np

import resnet as rs

def main():
    input_shape = (4, 28, 28, 3)
    print(input_shape[1:])

    # input configuration
    EPOCHS = 50
    BATCH_SIZE = 8
    NUM_CLASSES = 5
    image_height = 224
    image_width = 224
    channels = 3
    save_model_dir = "saved_model/model"
    dataset_dir = "dataset/"
    train_dir = dataset_dir + "train"
    valid_dir = dataset_dir + "valid"
    test_dir = dataset_dir + "test"

    model = ResNetTypeI(NUM_CLASSES, layer_params=[2, 2, 2, 2])
    model.summary()

if __name__== "__main__":
    print("Tensorflow version: %s" % tf.__version__)
    print("GPU device: %s" % tf.test.gpu_device_name())
    main()