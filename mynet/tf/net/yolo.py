import numpy as np
import os

from . import basenet

class YOLO(basenet.BaseNet):
    def __init__(self):
        self.pretrained_collection = []
        self.trainable_collection = []

    def leaky_relu(x, alpha = 0.1):
        """
            Formula 2 in the paper
            if x > 0
                return x
            else
                return x * alpha
        """
        if x > 0.0:
            return x
        else:
            return x * alpha

    def conv2(self, scope, input, kernel_size, stride=1):
        """
            params:
                kernel_size
        """
        with tf.variable_scope(scope) as scope:
            kernel = self.variable_with_weight_decay('weights',
                                                    shape=kenel_size,
                                                    stddev=5e-2,
                                                    wd=self.weight_decay,
                                                    pretrain=pretrain,
                                                    train=train)
            conv = tf.nn.conv2d(input, kernel, [1, stride, stride, 1], padding="SAME")
            biases = self.variable_on_cpu('biases', kernel_size[3:], tf.constant_initializer(0.0), pretrain, train)
            net_value = tf.nn.bias_add(conv, biases)
            conv1 = self.leaky_relu(net_value)

        return conv1

    def max_pool(self, input, kernel_size, stride):
        """
            Params:
                input: 4-D tensor
                kernel_size: [height, width]
                stride: scalar int32
            Return:
                4-D tensor [batch_size, height/stride, width/stride, depth]
        """
        return tf.nn.max_pool(input, ksize=[1, kernel_size[0], kernel_size[1], 1], strides=[1, stride, stride, 1],
                        padding="SAME")

    def ful_conn(self, scope, input, in_dimension, out_dimension, leaky=True, pretrain=True, train=True):
        """
        Fully Connected layer
        Args:
          scope: variable_scope name
          input: [batch_size, ???]
          out_dimension: int32
        Return:
          output: 2-D tensor [batch_size, out_dimension]
        """
        with tf.variable_scope(scope) as scope:
            reshape = tf.reshape(input, [tf.shape(input)[0], -1])
            weights = self.variable_with_weight_decay("weights", shape=[in_dimension, out_dimension],
                            stddev=0.04, wd=self.weight_decay, pretrain=pretrain, train=train)
            biases = self.variable_on_cpu('biases', [out_dimension], tf.constant_initializer(0.0), pretrain, train)
            local = tf.matmul(reshape, weights) + biases
            if leaky:
                local = self.leaky_relu(local)
            else:
                local = tf.identity(local, name=scope.name)

        return local
