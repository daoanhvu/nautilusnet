import tensorflow as tf

class BatchNormalization(tf.keras.layers.BatchNormalization):
  def call(self, x, training=False):
    if training is None:
      training = tf.constant(False)
    training = tf.logical_and(training, self.trainable)
    return super().call(x, training)