import tensorflow as tf
import numpy as np

image = tf.keras.preprocessing.image.load_img("../../rcnn/beagle.png", target_size=(64, 64))
print(image)

input_arr = tf.keras.preprocessing.image.img_to_array(image)
# print(input_arr)

# Convert the image into a batch
input_arr = np.array([input_arr])
kernel_in = np.array([ 
                        [[[2, 0.1, 1, 1]], [[1, 3, 0.2, 1]]],
                        [[[0, 0.3, 1, 1]], [[1, 1, 0.4, 1]]] 
                      ])

x = tf.constant(input_arr, dtype=tf.float32)
kernel = tf.constant(kernel_in, dtype=tf.float32)

# Reshape the input and kernel so that it will be in 4-D shape
x = tf.reshape(x, [1, 64, 64, 3])
kernel = tf.reshape(kernel, [4, 4, 1, 1])

tf.nn.conv2d(x, kernel, strides=[1, 1, 1, 1], padding='VALID')

print(input_arr.shape)
print(x_in.shape)
print(kernel_in.shape)