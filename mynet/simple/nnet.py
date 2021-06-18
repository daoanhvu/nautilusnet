import tensorflow as tf
import numpy as np
import pathlib

from resnet import ResNetTypeI
from data_utils import train_ds

def train_ds(path, subset, img_h, img_w, batch_size):
    path = pathlib.Path(path)
    # image_count = len(list(path.glob('*/*.jpg')))
    # print(image_count)
    return tf.keras.preprocessing.image_dataset_from_directory(
        path,
        validation_split=0.2,
        subset=subset,
        seed=123,
        image_size=(img_h, img_w),
        batch_size=batch_size
    )


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

    # GPU settings
    gpus = tf.config.experimental.list_physical_devices('GPU')
    if gpus:
        for gpu in gpus:
            tf.config.experimental.set_memory_growth(gpu, True)

    # Prepare dataset
    trainds = train_ds('/Users/vdao/Downloads/flower_photos', 'training', image_height, image_width, batch_size=BATCH_SIZE)
    valds = train_ds('/Users/vdao/Downloads/flower_photos', 'validation', image_height, image_width, batch_size=BATCH_SIZE)

    class_names = trainds.class_names
    print(class_names)

    # Configure the dataset for performance
    trainds = trainds.cache().prefetch(buffer_size=tf.data.AUTOTUNE)
    valds = valds.cache().prefetch(buffer_size=tf.data.AUTOTUNE)

    # Create a model
    model = ResNetTypeI(NUM_CLASSES, layer_params=[2, 2, 2, 2])
    model.build(input_shape=(None, image_height, image_width, channels))
    model.summary()

    # define loss and optimizer
    loss_object = tf.keras.losses.SparseCategoricalCrossentropy()
    optimizer = tf.keras.optimizers.Adadelta()

    train_loss = tf.keras.metrics.Mean(name='train_loss')
    train_accuracy = tf.keras.metrics.SparseCategoricalAccuracy(name='train_accuracy')

    valid_loss = tf.keras.metrics.Mean(name='valid_loss')
    valid_accuracy = tf.keras.metrics.SparseCategoricalAccuracy(name='valid_accuracy')

    model.compile(optimizer=optimizer, loss=loss_object, metrics=['accuracy'])

    model.fit(trainds, validation_data=valds, epochs=3)

    model.save_weights(filepath=config.save_model_dir, save_format='tf')

if __name__== "__main__":
    print("Tensorflow version: %s" % tf.__version__)
    print("GPU device: %s" % tf.test.gpu_device_name())
    main()