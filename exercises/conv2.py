import tensorflow as tf
import numpy as np

kernel = tf.constant([
    [1.0, 0, 1.0],
    [0.5, 0, 0.5],
    [1.0, 0, 2.0],
], dtype=tf.float32, name='kernel')
print(kernel)
a = tf.split(kernel, num_or_size_splits=3, axis=1)
print(a[0])
a0 = tf.reshape(a[0], shape=(1,3))
print(a0)


al = tf.slice(kernel, begin=(0, 0), size=(3, 1))
print(al)

al1 = tf.slice(kernel, begin=(0, 1), size=(3, 1))
print(al1)

# x = tf.constant([
#     [4, 3, 1, 0],
#     [2, 1, 0, 1],
#     [1, 2, 4, 1],
#     [3, 1, 0, 2]
# ], dtype=tf.float32, name='x')
# print(x.shape)
# kernel = tf.reshape(kernel, [3, 3, 1, 1], name='kernel')
# x = tf.reshape(x, [1, 4, 4, 1], name='x')
# print(x.shape)
# # VALID means no padding
# res = tf.squeeze(tf.nn.conv2d(x, kernel, [1, 1, 1, 1], "VALID"))

# print(res)