import tensorflow as tf
import numpy as np

image = tf.keras.preprocessing.image.load_img("../../rcnn/beagle.png", target_size=(64, 128))
print(image)

input_arr = tf.keras.preprocessing.image.img_to_array(image)
print(input_arr)

# Convert the image into a batch
input_arr = np.array([input_arr])
print(input_arr.shape)