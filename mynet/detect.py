import os
os.environ['TF_CPP_MIN_LOG_LEVEL'] = '2'
import pathlib
import time
import argparse
from PIL import Image
import numpy as np
import tensorflow as tf
from object_detection.utils import label_map_util
from object_detection.utils import config_util
from object_detection.builders import model_builder
from object_detection.utils import visualization_utils as viz_utils
import matplotlib.pyplot as plt
import cv2
import warnings
warnings.filterwarnings('ignore')   # Suppress Matplotlib warnings

def load_image_into_np_array(path):
    return np.array(Image.open(path))

def detect_with_saved_model():
    tf.get_logger().setLevel('ERROR')
    print(tf.__version__)
    current_dir = str(os.getcwd())
    IMAGES_PATH = current_dir + '/TensorFlow/models/research/object_detection/test_images'
    print(IMAGES_PATH)
    file_names = ['image1.jpg', 'image2.jpg']
    image_paths = []

    for filename in file_names:
        image_paths.append(IMAGES_PATH + '/' + filename)

    PATH_TO_LABELS = current_dir + '/TensorFlow/models/research/object_detection/data/mscoco_label_map.pbtxt'
    category_index = label_map_util.create_category_index_from_labelmap(PATH_TO_LABELS,
                                                                    use_display_name=True)

    PATH_TO_SAVED_MODEL = "TensorFlow/ssd_mobilenet_v2_fpnlite_320x320_coco17_tpu-8/saved_model"
    detect_fn = tf.saved_model.load(PATH_TO_SAVED_MODEL)

    for img_path in image_paths:
        image_np = load_image_into_np_array(img_path)
        print(img_path)
        # The input needs to be a tensor, convert it using `tf.convert_to_tensor`.
        input_tensor = tf.convert_to_tensor(image_np)
        # The model expects a batch of images, so add an axis with `tf.newaxis`.
        input_tensor = input_tensor[tf.newaxis, ...]

        # input_tensor = np.expand_dims(image_np, 0)
        detections = detect_fn(input_tensor)
        # All outputs are batches tensors.
        # Convert to numpy arrays, and take index [0] to remove the batch dimension.
        # We're only interested in the first num_detections.
        num_detections = int(detections.pop('num_detections'))
        detections = {key: value[0, :num_detections].numpy()
                    for key, value in detections.items()}
        print(detections)
        detections['num_detections'] = num_detections

        # detection_classes should be ints.
        detections['detection_classes'] = detections['detection_classes'].astype(np.int64)

        image_np_with_detections = image_np.copy()
        # print(detections['detection_classes'])
        viz_utils.visualize_boxes_and_labels_on_image_array(
          image_np_with_detections,
          detections['detection_boxes'],
          detections['detection_classes'],
          detections['detection_scores'],
          category_index,
          use_normalized_coordinates=True,
          max_boxes_to_draw=100,
          min_score_thresh=.50,
          agnostic_mode=False)

        plt.figure()
        plt.imshow(image_np_with_detections)
        print('Done')
    plt.show()

def detect_with_checkpoint(pipeline_path, chkpt_path, labelmap_path, image):
    configs = config_util.get_configs_from_pipeline_file(pipeline_path)
    model_config = configs['model']
    detection_model = model_builder.build(model_config=model_config, is_training=False)

    #Load checkpoint and restore it
    chkpt = tf.train.Checkpoint(detection_model)
    chkpt.restore(chkpt_path).expect_partial()
    
    # Convert image data to tensor
    arr_image = np.asarray(image)
    input_tensor = tf.convert_to_tensor(np.expand_dims(arr_image, 0), dtype=tf.float32)

    # Start detection process
    preprocessed_image, shapes = detection_model.preprocess(input_tensor)
    prediction_dict = detection_model.predict(preprocessed_image, shapes)
    detections = detection_model.postprocess(prediction_dict, shapes)
    # All outputs are batches tensors.
    # Convert to numpy arrays, and take index [0] to remove the batch dimension.
    # We're only interested in the first num_detections.
    num_detections = int(detections.pop('num_detections'))
    # num_detections = int(detections.pop('num_detections'))
    detections = {key: value[0, :num_detections].numpy() for key, value in detections.items()}
    detections['num_detections'] = num_detections

    # detection_classes should be ints.
    detections['detection_classes'] = detections['detection_classes'].astype(np.int64)

    detection_scores = detections['detection_scores']
    detection_classes = detections['detection_classes']
    detection_boxes = detections['detection_boxes']

    # Draw the output image with boxes
    height, width, _ = image.shape
    coordinate_dict = dict()
    category_index = label_map_util.create_category_index_from_labelmap(labelmap_path, use_display_name=True)
    print(detection_scores)
    print("Score list's length: " + str(detection_scores.shape))
    for i, score in enumerate(detection_scores):
        if score < 0.5:
            continue

        detection_classes[i] += 1
        if detection_classes[i] == 0:
            continue

        label = str(category_index[detection_classes[i]]['name'])
        ymin, xmin, ymax, xmax = detection_boxes[i]
        print(detection_boxes[i])
        real_xmin, real_ymin, real_xmax, real_ymax = int(xmin * width), int(ymin * height), int(xmax * width), int(
            ymax * height)

        # curr = real_xmax * real_ymax - real_ymin * real_xmin
        # status = check_overlap(curr, li)
        # if status == 1:
        #     continue
        # li.append(real_xmax * real_ymax - real_ymin * real_xmin)

        cv2.rectangle(image, (real_xmin, real_ymin), (real_xmax, real_ymax), (0, 255, 0), 2)
        cv2.putText(image, label, (real_xmin, real_ymin), cv2.FONT_HERSHEY_SIMPLEX, color=(0, 0, 255), fontScale=0.5)
        coordinate_dict[label] = (real_xmin, real_ymin, real_xmax, real_ymax)
    return image, coordinate_dict


if __name__=='__main__':
    parser = argparse.ArgumentParser(
       description=
       'Detect object using saved_mnode or load from checkpoint',
       formatter_class=argparse.RawDescriptionHelpFormatter)
    
    parser.add_argument('--m', type=str, required=True,
       help='Method to use saved_model or checkpoint')

    parser.add_argument('--image', type=str,
       help='Method to use saved_model or checkpoint')

    parser.add_argument('--label', type=str,
       help='Method to use saved_model or checkpoint')

    parser.add_argument('--checkpoint', type=str,
       help='Method to use saved_model or checkpoint')

    parser.add_argument('--p', type=str,
       help='Method to use saved_model or checkpoint')

    args = parser.parse_args()

    if args.m == 'saved_model':
        detect_with_saved_model()
    elif args.m == 'checkpoint':
        image_path = args.image
        image = cv2.imread(image_path)
        image, _ = detect_with_checkpoint(args.p, args.checkpoint, args.label, image)
        # cv2.imwrite('corner_test.png', image)
        # cv2.imshow('test', image)
        # cv2.waitKey(0)
    else:
        print('Wrong method!')
