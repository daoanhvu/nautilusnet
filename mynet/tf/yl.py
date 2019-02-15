import sys
sys.path.append('./')
import os
import tensorflow as tf
import numpy as np
import cv2
import dataset.dataset3d as dt

CLASSES_NAME = ['teddy', 'ant', 'airplane']
NUM_CLASSES = len(CLASSES_NAME)
NUM_GRID = 7
LEARNING_RATE = 1e-4
LAMBDA_COORD = 5
LAMBDA_NOOBJ = 0.5
THRESHOLD = 0.2
########################################################
START_IDX_PROBS = 0
END_IDX_PROBS = (NUM_CLASSES) * (NUM_GRID**2)
START_IDX_CONFIDENCES = END_IDX_PROBS
END_IDX_CONFIDENCES = START_IDX_CONFIDENCES + (NUM_GRID**2)*2
START_IDX_BBOXES = END_IDX_CONFIDENCES
END_IDX_BBOXES = START_IDX_BBOXES + (NUM_GRID**2)*2*4
global_batch_size = 5

NUM_EPOCH = 10

# =============== Utility functions ======================
def sqrt_wh(box):
  """
  Take the square root of wh regardless of pred or true boxes given
  INPUTS:
  - box:
  OUTPUTS:
  - box_new:
  """
  if len(box.get_shape().as_list()) == 4:
    box_new = tf.concat(3, [box[:, :, :, : 2], tf.sqrt(box[:, :, :, 2 :])])
  else:
    print("LABELS HAVE WRONG SHAPE !!!")
  return box_new


def square_wh(boxes):
  """
  Take the square of wh regardless of pred or true boxes given
  INPUTS:
  - boxes: n-d array of shape ?????
  OUTPUTS:
  -
  """
  # boxes_wh_squared = tf.concat(1, [boxes[:, :2], tf.square(boxes[:, 2:]) ])
  boxes_wh_squared = tf.concat([boxes[:, :2], tf.square(boxes[:, 2:])], 1)
  return boxes_wh_squared

def unionTF(a,b):
    """
    Calculates the union of two rectangles, a and b, where each is an array [x,y,w,h]
    """
    w1 = tf.slice(a,[2,],[1,])
    h1 = tf.slice(a,[3,],[1,])
    w2 = tf.slice(b,[2,],[1,])
    h2 = tf.slice(b,[3,],[1,])
    union = tf.mul(w1,h1)
    union += tf.mul(w2,h2)
    union -= intersectionTF(a,b)
    return union #tf.to_float(union)

def intersectionTF(a,b):
    """
    Calculates the intersection of two rectangles, a and b, where each is an array [x,y,w,h]
    tf.truediv(x, y) # divides x / y elementwise WITH FLOATING PT RESULTS
    """
    two = tf.constant(2,dtype=tf.float32)
    zero = tf.constant(0,dtype=tf.float32)
    x1 = tf.slice(a,[0,],[1,])
    y1 = tf.slice(a,[1,],[1,])
    w1 = tf.slice(a,[2,],[1,])
    h1 = tf.slice(a,[3,],[1,])
    x2 = tf.slice(b,[0,],[1,])
    y2 = tf.slice(b,[1,],[1,])
    w2 = tf.slice(b,[2,],[1,])
    h2 = tf.slice(b,[3,],[1,])

    topA    = tf.add(y1,tf.div(h1,two) )
    bottomA = tf.sub(y1,tf.div(h1,two) )
    rightA  = tf.add(x1,tf.div(w1,two) )
    leftA   = tf.sub(x1,tf.div(w1,two) )

    topB    = tf.add(y2,tf.div(h2,two) )
    bottomB = tf.sub(y2,tf.div(h2,two) )
    rightB  = tf.add(x2,tf.div(w2,two) )
    leftB   = tf.sub(x2,tf.div(w2,two) )

    xOverlap=tf.maximum(zero,tf.minimum(rightA,rightB) - tf.maximum(leftA,leftB) )
    yOverlap=tf.maximum(zero,tf.minimum(topA,topB) - tf.maximum(bottomA,bottomB) )
    return tf.mul(xOverlap, yOverlap)

def iouTF(a,b):
    """
    Returns the intersection over union of two rectangles, a and b, where each is an array [x,y,w,h]
    """
    return intersectionTF(a,b)/unionTF(a,b)

class NautilusDark:
    def __init__(self):
        self.height = 448
        self.width = 448
        self.weight_decay = 0.0000005
        self.B = 2
        self.S = 7
        self.lr = 0.00015
        self.input_layer = tf.placeholder(tf.float32, (1, self.height, self.width, 3))
        # self.input_layer = tf.placeholder(tf.float32, (1, self.height, self.width, 3))
        self.gt_conf = tf.placeholder(tf.float32, shape=[49,4],name='GT_CONF')
        self.gt_classes = tf.placeholder(tf.float32,shape=[49,NUM_CLASSES],name='GT_CLASSES')
        self.ind_obj_i = tf.placeholder(tf.float32, shape=[7*7],name='ind_obj_i')
        # dropout prob is only set <1 during training
        self.dropout_prob = tf.placeholder(tf.float32)
        self.gt_boxes_j0 = tf.placeholder(tf.float32,shape=[49,4],name= 'gt_boxes_j0')
        self.create()

    def create(self):

        # Build the net
        conv1 = self.conv2d('conv1', self.input_layer, [7, 7, 3, 64], stride=2)
        # maxpool1 = self.max_pool(conv1, [2, 2], 2)
        maxpool1 = tf.nn.max_pool(conv1, ksize=[1, 2, 2, 1], strides=[1, 2, 2, 1], padding="SAME")

        conv2 = self.conv2d('conv2', maxpool1, [3, 3, 64, 192], stride=1)
        # maxpool2 = self.max_pool(conv2, [2, 2], 2)
        maxpool2 = tf.nn.max_pool(conv2, ksize=[1, 2, 2, 1], strides=[1, 2, 2, 1], padding="SAME")

        conv3 = self.conv2d('conv3', maxpool2, [1, 1, 192, 128], stride=1)

        conv4 = self.conv2d('conv4', conv3, [3, 3, 128, 256], stride=1)

        conv5 = self.conv2d('conv5', conv4, [1, 1, 256, 256], stride=1)

        conv6 = self.conv2d('conv6', conv5, [3, 3, 256, 512], stride=1)

        # maxpool3 = self.max_pool(conv6, [2, 2], 2)
        maxpool3 = tf.nn.max_pool(conv6, ksize=[1, 2, 2, 1], strides=[1, 2, 2, 1], padding="SAME")

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

        # maxpool4 = self.max_pool(conv18, [2, 2], 2)
        maxpool4 = tf.nn.max_pool(conv18, ksize=[1, 2, 2, 1], strides=[1, 2, 2, 1], padding="SAME")

        conv19 = self.conv2d('conv19', maxpool4, [1, 1, 1024, 512], stride=1)
        conv20 = self.conv2d('conv20', conv19, [3, 3, 512, 1024], stride=1)

        conv21 = self.conv2d('conv21', conv20, [1, 1, 1024, 512], stride=1)
        conv22 = self.conv2d('conv22', conv21, [3, 3, 512, 1024], stride=1)

        conv23 = self.conv2d('conv23', conv22, [3, 3, 1024, 1024], stride=1)

        conv24 = self.conv2d('conv24', conv23, [3, 3, 1024, 1024], stride=2)

        conv25 = self.conv2d('conv25', conv24, [3, 3, 1024, 1024], stride=1)

        conv26 = self.conv2d('conv26', conv25, [3, 3, 1024, 1024], stride=1)

        #Fully connected layer
        local1 = self.full_connect('local1', conv26, 49 * 1024, 4096)

        dropout1 = tf.nn.dropout(local1, keep_prob=self.dropout_prob)

        d_local = self.S * self.S * (self.B * 5 + NUM_CLASSES)
        local2 = self.full_connect('local2', dropout1, 4096, d_local, leaky=False)

        # print("local2 shape: ", tf.shape(local2))
        # last_reshape = tf.reshape(local2, [tf.shape(local2)[0], self.S, self.S, self.B * 5 + NUM_CLASSES])

        self.output_layer = local2
        # print("BBOX shapes: ", self.output_layer)

        self.class_probs = self.output_layer[:, START_IDX_PROBS: END_IDX_PROBS ]
        self.confidences = self.output_layer[:, START_IDX_CONFIDENCES: END_IDX_CONFIDENCES ]
        self.bboxes = self.output_layer[:, START_IDX_BBOXES : END_IDX_BBOXES]

        self.class_probs = tf.reshape( self.class_probs,shape=[NUM_GRID*NUM_GRID, NUM_CLASSES])
        self.confidences = tf.reshape( self.confidences,shape=[NUM_GRID*NUM_GRID, 2])
        self.bboxes = tf.reshape(self.bboxes, shape=[NUM_GRID*NUM_GRID, 2*4])

        self.loss = self.yolo_loss(self.class_probs, self.confidences, self.bboxes, self.gt_conf,self.gt_classes, self.ind_obj_i, self.gt_boxes_j0)

        self.train_op = tf.train.GradientDescentOptimizer(self.lr).minimize(self.loss)

    def yolo_loss(self, pred_classes, pred_conf, pred_boxes, gt_conf, gt_classes, ind_obj_i, gt_boxes_j0):
        """
        As a simplification, right now I only match one bounding box predictor
        to 1 ground truth, in each grid cell when compute iou, need to have
        pred boxes norm to image, wh as is when compute coord loss, need to
        have true boxes norm to grid, wh square root

        Confidence computed as Pr(Object) * IOUtruth_pred
        If no object exists in that cell, the confidence scores should be zero.
        Otherwise we want the confidence score to equal the intersection over union (IOU)
        between the predicted box and the ground truth.
        Finally the confidence prediction represents the IOU between the predicted box
        and any ground truth box.

        We assign one predictor to be "responsible" for predicting an object
        based on which prediction has the highest current IOU with the ground truth.

        NEW INPUTS:
        - pred_classes: 49 x NUM_CLASSES
        - pred_conf: 49 x 2 array
        - pred_boxes: 49 x 8 array
        - gt_boxes_j0; 49 x 4
        - I'M IGNORING THIS FOR NOW IN THE SIMPLIFICATION -- gt_boxes_j1: 49 x 4
        - gt_conf: 49 x 4, values in each of 4 columns are identical (tiled/repmatted)
        - gt_classes: 49 x NUM_CLASSES
        - ind_obj_i: 49 x NUM_CLASSES, indicating if that grid cell contains any object
        """
        pred_conf_j0 = pred_conf[:,0]
        pred_conf_j1 = pred_conf[:,1]
        ############ BOX LOSS ##################################################
        pred_boxes = tf.reshape( pred_boxes, shape=[49,2,4] )
        pred_boxes_j0 = pred_boxes[:,0,:] # 49 x 4 array
        pred_boxes_j1 = pred_boxes[:,1,:] # 49 x 4 array
        pred_boxes_j0 = tf.multiply( pred_boxes_j0 , gt_conf ) # multiply by 1s or 0s
        pred_boxes_j1 = tf.multiply( pred_boxes_j1 , gt_conf ) # multiply by 1s or 0s
        # NOW the predictions in wrong cells are zeroed out
        j0_coord_loss = tf.reduce_sum(tf.square(pred_boxes_j0 - gt_boxes_j0), reduction_indices=[1] )
        # print("GROUND TRUTH BOXES: " , gt_boxes_j0)
        squared_gt_boxes_j0 = square_wh(gt_boxes_j0)
        squared_pred_boxes_j0 = square_wh(pred_boxes_j0)
        squared_pred_boxes_j1 = square_wh(pred_boxes_j1)
        # For each of the predicted boxes in each grid cell, we check if B_1 or B_2 has a higher IOU with GT
        #pbs_j0 = [pb for pb in tf.split(0, 49, squared_pred_boxes_j0 )] # BREAK INTO chunks of 4
        ious_j0 = self.compute_iou( squared_pred_boxes_j0 , squared_gt_boxes_j0 )
        #pbs_j1 = [pb for pb in tf.split(0, 49, squared_pred_boxes_j1 )] # BREAK INTO chunks of 4
        ious_j1 = self.compute_iou( squared_pred_boxes_j1 , squared_gt_boxes_j0 )
        mask_temp = tf.greater( ious_j0, ious_j1 )
        final_ious = tf.where(mask_temp, ious_j0, ious_j1 )
        j1_coord_loss = tf.reduce_sum(tf.square(pred_boxes_j1 - gt_boxes_j0), reduction_indices=[1] )
        box_loss = tf.where(mask_temp, j0_coord_loss, j1_coord_loss )
        box_loss = LAMBDA_COORD * tf.reduce_sum(box_loss, reduction_indices=[0])
        ##############################################################################

        ############ OBJECT LOSS ##################################################
        # NOW ZERO OUT THE predicted_CONFIDENCES if no object there
        pred_conf_j0 *= gt_conf[:,0]
        pred_conf_j1 *= gt_conf[:,0]
        # Now only one of those predictors is doing the work. Mask out the other
        j0_mask = tf.logical_and( mask_temp, tf.greater( pred_conf_j0, tf.zeros_like(pred_conf_j0)) )
        j1_mask = tf.logical_and( tf.greater( pred_conf_j1, tf.zeros_like(pred_conf_j1)), tf.logical_not(mask_temp))

        pred_conf_j0 = tf.where(j0_mask, pred_conf_j0, tf.zeros_like(pred_conf_j0) )
        pred_conf_j1 = tf.where(j1_mask, pred_conf_j1, tf.zeros_like(pred_conf_j1) )

        obj_ious = final_ious * gt_conf[:,0]
        gt_conf_j0 = tf.where(j0_mask, obj_ious, tf.zeros_like(pred_conf_j0) )
        gt_conf_j1 = tf.where(j1_mask, obj_ious, tf.zeros_like(pred_conf_j0) )

        j0_obj_loss = tf.square( pred_conf_j0 - gt_conf_j0 )
        j1_obj_loss = tf.square( pred_conf_j1 - gt_conf_j1 )

        obj_loss = tf.reduce_sum( j0_obj_loss + j1_obj_loss, reduction_indices=[0])
        ##############################################################################

        ############ NO-OBJECT LOSS ##################################################
        gt_conf_noobj_mask = tf.greater( gt_conf, tf.zeros_like(gt_conf) )
        # gt_conf_noobj = tf.select( gt_conf_noobj_mask, tf.zeros_like(gt_conf), tf.ones_like(gt_conf)) # opposite of gt_conf
        gt_conf_noobj = tf.where( gt_conf_noobj_mask, tf.zeros_like(gt_conf), tf.ones_like(gt_conf)) # opposite of gt_conf

        j0_mask_noobj = tf.logical_not( j0_mask )
        j1_mask_noobj = tf.logical_not( j1_mask )

        pred_conf_j0_noobj = tf.where(j0_mask_noobj, pred_conf[:,0], tf.zeros_like(pred_conf_j0) )
        pred_conf_j1_noobj = tf.where(j1_mask_noobj, pred_conf[:,1], tf.zeros_like(pred_conf_j1) )

        noobj_ious = final_ious * gt_conf_noobj[:,0]
        gt_conf_j0_noobj = tf.where(j0_mask_noobj, noobj_ious, tf.zeros_like(pred_conf_j0) )
        gt_conf_j1_noobj = tf.where(j1_mask_noobj, noobj_ious, tf.zeros_like(pred_conf_j0) )

        noobj_loss_j0 = tf.square( pred_conf_j0_noobj - gt_conf_j0_noobj )
        noobj_loss_j1 = tf.square( pred_conf_j1_noobj - gt_conf_j1_noobj )

        noobj_loss = LAMBDA_NOOBJ * tf.reduce_sum(noobj_loss_j0 + noobj_loss_j1, reduction_indices=[0])
        ##############################################################################

        ############ COMPLETED CLASS LOSS ############################################
        class_loss = gt_classes - pred_classes # both are 49 x NUM_CLASSES
        class_loss = tf.square(class_loss)
        class_loss = tf.reduce_sum(class_loss, reduction_indices=[1]) # along all classes
        class_loss = tf.multiply( ind_obj_i, class_loss) # both are 49 x 1
        class_loss = tf.reduce_sum(class_loss, reduction_indices=[0]) # along all boxes
        #############################################################################

        return box_loss + class_loss #+ noobj_loss#obj_loss # +


    def variable_on_cpu(self, name, shape, initializer, pretrain=True, train=True):
        with tf.device('/cpu:0'):
            var = tf.get_variable(name, shape, initializer=initializer, dtype=tf.float32)
        return var

    def variable_with_weight_decay(self, name, shape, stddev, wd, pretrain=True, train=True):
        var = self.variable_on_cpu(name, shape, tf.truncated_normal_initializer(stddev=stddev, dtype=tf.float32), pretrain, train)
        if wd is not None:
            weight_decay = tf.multiply(tf.nn.l2_loss(var), wd, name='weight_loss')
            tf.add_to_collection('losses', weight_decay)
        return var

    def conv2d(self, scope, input, kernel_size, stride=1, padding_="SAME", pretrain=True, train=True):
        with tf.variable_scope(scope) as scope:
            # kernel = self.variable_with_weight_decay('weights',
            #                                         shape=kernel_size,
            #                                         stddev=5e-2,
            #                                         wd=self.weight_decay,
            #                                         pretrain=pretrain,
            #                                         train=train)
            kernel = self.variable_on_cpu('weights',
                                                    shape=kernel_size,
                                                    initializer= tf.contrib.layers.xavier_initializer_conv2d(),
                                                    pretrain=pretrain,
                                                    train=train)
            conv = tf.nn.conv2d(input, kernel, [1, stride, stride, 1], padding=padding_)
            biases = self.variable_on_cpu('biases', kernel_size[3:], tf.constant_initializer(0.0), pretrain, train)
            net_value = tf.nn.bias_add(conv, biases)
            conv1 = self.leaky_relu(net_value)

        return conv1

    def leaky_relu(self, x, alpha = 0.1, dtype=tf.float32):
        """ if x > 0 return x   else return x * alpha """
        x = tf.cast(x, dtype=dtype)
        bool_mask = (x > 0)
        mask = tf.cast(bool_mask, dtype=dtype)
        return 1.0 * mask * x + alpha * (1 - mask) * x

    def compute_iou(self, box_pred, box_true):
      """
      tensorflow version of computing iou between the predicted box and the gt box
      INPUTS:
      - box_pred: Tensor of shape [?, 7, 7, 4], xy norm to image, wh as is
      - box_true: Tensor of shape [?, 1, 1, 4], xy norm to image, wh as is
      OUTPUTS:
      - iou: Tensor of shape [?,7,7,1], intersection over unit with each predicted bounding box
      """
      # print 'PRED BOX SHAPE: ', box_pred.get_shape().as_list()
      # print 'GT BOX SHAPE: ', box_true.get_shape().as_list()
      lr = tf.minimum(box_pred[ :, 0 ] + 0.5 * box_pred[ :,2 ],   \
                      box_true[ :, 0 ] + 0.5 * box_true[ :,2 ]) - \
           tf.maximum(box_pred[ :, 0 ] - 0.5 * box_pred[ :,2 ],   \
                      box_true[ :, 0 ] - 0.5 * box_true[ :,2 ])
      tb = tf.minimum(box_pred[ :, 1 ] + 0.5 * box_pred[ :,3 ],   \
                      box_true[ :, 1 ] + 0.5 * box_true[ :,3 ]) - \
           tf.maximum(box_pred[ :, 1 ] - 0.5 * box_pred[ :,3 ],   \
                      box_true[ :, 1 ] - 0.5 * box_true[ :,3 ])
      lr = tf.maximum(lr, lr * 0)
      tb = tf.maximum(tb, tb * 0)
      intersection = tf.multiply(tb, lr)
      union = tf.subtract(tf.multiply(box_pred[ :, 2], box_pred[ :, 3 ]) +  \
                     tf.multiply(box_true[ :, 2], box_true[ :, 3 ]), intersection)
      iou = tf.div(intersection, union)

      return iou

        #saver1.restore(sess, self.pretrain_path)
    def inference(self, sess, img):
        predictions = sess.run(self.output_layer, feed_dict = {self.input_layer: img, self.dropout_prob: 1})

        class_probs = predictions[:, START_IDX_PROBS: END_IDX_PROBS ]
        confidences = predictions[:, START_IDX_CONFIDENCES: END_IDX_CONFIDENCES ]
        bboxes = predictions[:, START_IDX_BBOXES : END_IDX_BBOXES]

        class_probs = tf.reshape(class_probs,shape=[NUM_GRID, NUM_GRID, NUM_CLASSES])
        confidences = tf.reshape(confidences,shape=[NUM_GRID, NUM_GRID, self.B])
        # bboxes = tf.reshape(bboxes, shape=[NUM_GRID*NUM_GRID,2*4])
        bboxes = tf.reshape(bboxes, shape=[NUM_GRID*NUM_GRID, 2, 4])
        return class_probs, confidences, bboxes

    def full_connect(self, scope, input, in_dimension, out_dimension, leaky=True, pretrain=True, train=True):
        """Fully connection layer
        Args:
          scope: variable_scope name
          input: [batch_size, ???]
          out_dimension: int32
        Return:
          output: 2-D tensor [batch_size, out_dimension]
        """
        with tf.variable_scope(scope) as scope:
          reshape = tf.reshape(input, [tf.shape(input)[0], -1])

          weights = self.variable_with_weight_decay('weights', shape=[in_dimension, out_dimension],
                              stddev=0.04, wd=self.weight_decay, pretrain=pretrain, train=train)
          biases = self.variable_on_cpu('biases', [out_dimension], tf.constant_initializer(0.0), pretrain, train)
          local = tf.matmul(reshape, weights) + biases

          if leaky:
            local = self.leaky_relu(local)
          else:
            local = tf.identity(local, name=scope.name)

        return local

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

def train(net, pretrained = False):
    # annotatedImages = dt.getData('/home/davu/projects/nautilusnet/data/textdata.txt')
    annotatedImages = dt.getData('/Volumes/data/projects/nautilusnet/data/textdata.txt')
    # saver = tf.train.Saver()
    saver = tf.train.Saver(write_version=tf.train.SaverDef.V2)
    with tf.Session() as sess:
        sess.run(tf.global_variables_initializer())
        if pretrained:
            saver.restore(sess, tf.train.latest_checkpoint("ckpt/nautilus/"))
            # saver.restore(sess, "ckpt/nautilus/yl.ckpt")

        for epoch in range(NUM_EPOCH):
            print("=== Start epoch ", epoch)
            minibatchIms, minibatchGT = dt.get_batch_data(annotatedImages, global_batch_size)
            gt_confs, gt_classes1, ind_obj_is, gt_boxes_j0s = minibatchGT
            # minibatchIms = np.expand_dims( minibatchIms, 0)
            # print(minibatchGT.shape)
            for step in range(global_batch_size):
                gt_conf = gt_confs[step].astype(np.float32)
                gt_classes = gt_classes1[step].astype(np.float32)
                ind_obj_i = ind_obj_is[step].astype(np.float32)
                gt_boxes_j0 = gt_boxes_j0s[step].astype(np.float32)
                image_i = minibatchIms[step, :, :, :]
                image_i = np.reshape(image_i, (1, 448, 448, 3))
                ## FEED DROPOUT 0.5 AT TRAIN TIME, 1.0 AT TEST TIME #######
                feed = { net.input_layer:image_i , net.gt_conf : gt_conf, \
                    net.gt_classes : gt_classes, net.ind_obj_i: ind_obj_i, \
                    net.dropout_prob: 0.5, net.gt_boxes_j0 : gt_boxes_j0 }
                _, trainLossVal = sess.run([net.train_op, net.loss] ,feed_dict=feed ) # net.train_op
                print("Loss at step {0}: {1}" .format(step, trainLossVal))

        print("Saving weights...")
        saver.save(sess, "ckpt/nautilus/yl.ckpt")

def main():
    net = NautilusDark()
    train(net)

if __name__=='__main__':
    main()
