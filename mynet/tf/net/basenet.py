import tensorflow as tf

class BaseNet(object):
    def variable_on_cpu(self, name, shape, initializer, pretrain=True, train=True):
        """
            Params:
                name of Variable
                initializer of Variable
            return variable tensor
        """
        with tf.device('/cpu:0'):
            var = tf.get_variable(name, shape, initializer=initializer, dtype=tf.float32)
            if pretrain:
                self.pretrained_collection.append(var)
            if train:
                self.trainable_collection.append(var)
        return var

    def variable_with_weight_decay(self, name, shape, stdev, wd, pretrain=True, train=True):
        """
            Params:
                name of Variable
            Returns:
                Variable Tensor
        """
        var = self.variable_on_cpu(name, shape,
            tf.truncated_normal_initializer(stddev=stddev, dtype=tf.float32), pretrain, train)
        if wd is not None:
            weight_decay = tf.multiply(tf.nn.l2_loss(var), wd, name='weight_loss')
            tf.add_to_collection('losses', weight_decay)

        return var
