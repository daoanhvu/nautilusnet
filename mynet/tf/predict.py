import sys
sys.path.append('./')
import os
import tensorflow as tf
import numpy as np

classes_name = ['teddy', 'ant', 'airplane']

def process_predicts(predicts):
  p_classes = predicts[0, :, :, 0:20]
  C = predicts[0, :, :, 20:22]
  coordinate = predicts[0, :, :, 22:]

  p_classes = np.reshape(p_classes, (7, 7, 1, 20))
  C = np.reshape(C, (7, 7, 2, 1))
  P = C * p_classes

  #print P[5,1, 0, :]
  index = np.argmax(P)
  index = np.unravel_index(index, P.shape)
  class_num = index[3]
  coordinate = np.reshape(coordinate, (7, 7, 2, 4))
  max_coordinate = coordinate[index[0], index[1], index[2], :]

  xcenter = max_coordinate[0]
  ycenter = max_coordinate[1]
  w = max_coordinate[2]
  h = max_coordinate[3]
  xcenter = (index[1] + xcenter) * (448/7.0)
  ycenter = (index[0] + ycenter) * (448/7.0)
  w = w * 448
  h = h * 448
  xmin = xcenter - w/2.0
  ymin = ycenter - h/2.0

  xmax = xmin + w
  ymax = ymin + h

  return xmin, ymin, xmax, ymax, class_num

def main():
    common_params = {"image_size":0, "num_classes":3, "batch_size":2}
    net_params = {'cell_size':7, 'boxes_per_cell':2, 'weight_decay':0.0005}
    net = YOLO(common_params, net_params, test=True)

    # 1 image, size=(448x448), 3 channels (R, G, B)
    image = tf.placeholder(tf.float32, (1, 448, 448, 3))
    predicts = net.inference(image)

    sess = tf.Session()

    image_data = cv2.imread('pose.jpg')
    resized_img = cv2.resize(image_data, (448, 448))
    np_img = cv2.cvtColor(resized_img, cv2.COLOR_BGR2RGB)


    np_img = np_img.astype(np.float32)

    np_img = np_img / 255.0 * 2 - 1
    np_img = np.reshape(np_img, (1, 448, 448, 3))

    saver = tf.train.Saver(net.trainable_collection)

    saver.restore(sess, 'models/pretrain/yolo_tiny.ckpt')

    np_predict = sess.run(predicts, feed_dict={image: np_img})

    xmin, ymin, xmax, ymax, class_num = process_predicts(np_predict)
    class_name = classes_name[class_num]
    cv2.rectangle(resized_img, (int(xmin), int(ymin)), (int(xmax), int(ymax)), (0, 0, 255))
    cv2.putText(resized_img, class_name, (int(xmin), int(ymin)), 2, 1.5, (0, 0, 255))
    cv2.imwrite('pose_out.jpg', resized_img)
    sess.close()

    print('Predicting object finished!')

if __name__=='__main__':
    main()
