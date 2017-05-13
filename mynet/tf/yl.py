import sys
sys.path.append('./')
import os
import tensorflow as tf
import numpy as np
import cv2

CLASSES_NAME = ['teddy', 'ant', 'airplane']
NUM_CLASSES = len(CLASSES_NAME)
global_batch_size = 2

class NautilusDark:
    def __init__(self):
        self.weight_decay = 0.0000005
        self.B = 2
        self.S = 7

    def createNet():
        # Build the net
        conv1 = self.conv2d('conv1', images, [7, 7, 3, 64], stride=2)
        maxpool1 = self.max_pool(conv1, [2, 2], 2)

        conv2 = self.conv2d('conv2', maxpool1, [3, 3, 64, 192], stride=1)
        maxpool2 = self.max_pool(conv2, [2, 2], 2)

        conv3 = self.conv2d('conv3', maxpool2, [1, 1, 192, 128], stride=1)

        conv4 = self.conv2d('conv4', conv3, [3, 3, 128, 256], stride=1)

        conv5 = self.conv2d('conv5', conv4, [1, 1, 256, 256], stride=1)

        conv6 = self.conv2d('conv5', conv5, [3, 3, 256, 512], stride=1)

        maxpool3 = self.max_pool(conv6, [2, 2], 2)

        conv7 = self.conv2d('conv7', maxpool3, [1, 1, 512, 256], stride=1)
        conv8 = self.conv2d('conv8', conv7, [3, 3, 256, 512], stride=1)

        conv9 = self.conv2d('conv9', conv8, [1, 1, 512, 256], stride=1)
        conv10 = self.conv2d('conv10', conv9, [3, 3, 256, 512], stride=1)

        conv11 = self.conv2d('conv11', conv10, [1, 1, 512, 256], stride=1)
        conv12 = self.conv2d('conv12', conv11, [3, 3, 256, 512], stride=1)

        conv13 = self.conv2d('conv13', conv12, [1, 1, 512, 256], stride=1)
        conv14 = self.conv2d('conv14', conv13, [3, 3, 256, 512], stride=1)

        conv15 = self.conv2d('conv15', conv14, [1, 1, 512, 256], stride=1)
        conv16 = self.conv2d('conv16', conv15, [3, 3, 256, 512], stride=1)

        conv17 = self.conv2d('conv17', conv16, [1, 1, 512, 512], stride=1)

        conv18 = self.conv2d('conv18', conv17, [3, 3, 512, 1024], stride=1)

        maxpool4 = self.max_pool(conv18, [2, 2], 2)

        for i in range(2):
          temp_conv = self.conv2d('conv' + str(conv_num), temp_conv, [1, 1, 1024, 512], stride=1)
          temp_conv = self.conv2d('conv' + str(conv_num), temp_conv, [3, 3, 512, 1024], stride=1)

        temp_conv = self.conv2d('conv' + str(conv_num), temp_conv, [3, 3, 1024, 1024], stride=1)
        conv_num += 1

        temp_conv = self.conv2d('conv' + str(conv_num), temp_conv, [3, 3, 1024, 1024], stride=2)
        conv_num += 1

        #
        temp_conv = self.conv2d('conv' + str(conv_num), temp_conv, [3, 3, 1024, 1024], stride=1)
        conv_num += 1

        temp_conv = self.conv2d('conv' + str(conv_num), temp_conv, [3, 3, 1024, 1024], stride=1)
        conv_num += 1


        #Fully connected layer
        local1 = self.local('local1', temp_conv, 49 * 1024, 4096)


        local1 = tf.nn.dropout(local1, keep_prob=0.5)

        d_local = self.S * self.S * (self.B * 5 + NUM_CLASSES)
        local2 = self.local('local2', local1, 4096, d_local, leaky=False)

        local2 = tf.reshape(local2, [tf.shape(local2)[0], self.S, self.S, self.B * 5 + NUM_CLASSES])

        out = local2


    def variable_on_cpu(self, name, shape, initializer, pretrain=True, train=True):
        with tf.device('/cpu:0'):
            var = tf.get_variable(name, shape, initializer=initializer, dtype=tf.float32)
        return var

    def variable_with_weight_decay(self, name, shape, stddev, wd, pretrain=True, train=True):
        var = variable_on_cpu(name, shape,
            tf.truncated_normal_initializer(stddev=stddev, dtype=tf.float32), pretrain, train)
        if wd is not None:
            weight_decay = tf.multiply(tf.nn.l2_loss(var), wd, name='weight_loss')
            tf.add_to_collection('losses', weight_decay)
        return var

    def conv2d(self, scope, input, kernel_size, stride=1, padding_="SAME", pretrain=True, train=True):
        with tf.variable_scope(scope) as scope:
            kernel = self.variable_with_weight_decay('weights',
                                                    shape=kernel_size,
                                                    stddev=5e-2,
                                                    wd=global_weight_decay,
                                                    pretrain=pretrain,
                                                    train=train)
            conv = tf.nn.conv2d(input, kernel, [1, stride, stride, 1], padding=padding_)
            biases = self.variable_on_cpu('biases', kernel_size[3:], tf.constant_initializer(0.0), pretrain, train)
            net_value = tf.nn.bias_add(conv, biases)
            conv1 = self.leaky_relu(net_value)

        return conv1

    def leaky_relu(x, alpha = 0.1, dtype=tf.float32):
        """ if x > 0 return x   else return x * alpha """
        x = tf.cast(x, dtype=dtype)
        bool_mask = (x > 0)
        mask = tf.cast(bool_mask, dtype=dtype)
        return 1.0 * mask * x + alpha * (1 - mask) * x

    def solve():
        pretrained_collection = []
        trainable_collection = []
        #saver1 = tf.train.Saver(pretrained_collection, write_version=tf.train.SaverDef.V2)
        #saver2 = tf.train.Saver(trainable_collection, write_version=tf.train.SaverDef.V2)
        init =  tf.global_variables_initializer()
        summary_op = tf.summary.merge_all()
        sess = tf.Session()
        sess.run(init)

        #saver1.restore(sess, self.pretrain_path)

    def loss(predictions):
        class_loss = tf.constant(0, tf.float32)
        object_loss = tf.constant(0, tf.float32)
        noobject_loss = tf.constant(0, tf.float32)
        coord_loss = tf.constant(0, tf.float32)
        loss = [0, 0, 0, 0]
        for i in range(batch_size):
            predict = predicts[i, :, :, :]
        return 0

def process_image(record):
    image = cv2.imread(record[0])
    image = cv2.cvtColor(image, cv2.COLOR_BGR2RGB)
    h = image.shape[0]
    w = image.shape[1]
    width_rate = 448 * 1.0 / w
    height_rate = 448 * 1.0 / h
    image = cv2.resize(image, (448, 448))
    labels = [[0, 0, 0, 0, 0]] * 18
    i = 1
    object_num = 0
    while i < len(record):
      xmin = record[i]
      ymin = record[i + 1]
      xmax = record[i + 2]
      ymax = record[i + 3]
      class_num = record[i + 4]

      xcenter = (xmin + xmax) * 1.0 / 2 * width_rate
      ycenter = (ymin + ymax) * 1.0 / 2 * height_rate

      box_w = (xmax - xmin) * width_rate
      box_h = (ymax - ymin) * height_rate

      labels[object_num] = [xcenter, ycenter, box_w, box_h, class_num]
      object_num += 1
      i += 5
      if object_num >= 18:
        break
    return [image, labels, object_num]

def main():


    solve()
    record = "/home/davu/projects/nautilusnet/data/teddy_pose_0.jpg"
    print(classes_name)

if __name__=='__main__':
    main()
