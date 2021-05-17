import tensorflow as tf
import numpy as np
import pathlib

def load_image(img_path):
    normalization_layer = tf.keras.layers.experimental.preprocessing.Rescaling(1./255)


def train_ds(path, img_h, img_w, batch_size):
    path = pathlib.Path(path)
    image_count = len(list(path.glob('*/*.jpg')))
    print(image_count)
    return tf.keras.preprocessing.image_dataset_from_directory(
        path,
        validation_split=0.2,
        subset="training",
        seed=123,
        image_size=(img_h, img_w),
        batch_size=batch_size
    )
