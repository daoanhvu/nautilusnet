import sys
sys.path.append('./')
import os
import tensorflow as tf
import numpy as np
import cv2

classes_name = ['teddy', 'ant', 'airplane']

def variable_on_cpu(self, name, shape, initializer, pretrain=True, train=True):
    with tf.device('/cpu:0'):
        var = tf.get_variable(name, shape, initializer=initializer, dtype=tf.float32)
        if pretrain:
            self.pretrained_collection.append(var)
        if train:
            self.trainable_collection.append(var)
    return var

def variable_with_weight_decay(self, name, shape, stddev, wd, pretrain=True, train=True):
    var = self.variable_on_cpu(name, shape,
        tf.truncated_normal_initializer(stddev=stddev, dtype=tf.float32), pretrain, train)
    if wd is not None:
        weight_decay = tf.multiply(tf.nn.l2_loss(var), wd, name='weight_loss')
        tf.add_to_collection('losses', weight_decay)
    return var

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




def loss():
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
