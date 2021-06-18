import os
import tensorflow as tf



MODEL_DIR = '/Users/vdao/Documents/projects/demo/nautilusnet/models/TF'

CUSTOM_MODEL_NAME = 'my_ssd_mobnet' 
PRETRAINED_MODEL_NAME = 'ssd_mobilenet_v2_fpnlite_320x320_coco17_tpu-8'
PRETRAINED_MODEL_URL = 'http://download.tensorflow.org/models/object_detection/tf2/20200711/ssd_mobilenet_v2_fpnlite_320x320_coco17_tpu-8.tar.gz'
TF_RECORD_SCRIPT_NAME = 'generate_tfrecord.py'
LABEL_MAP_NAME = 'label_map.pbtxt'

def main():
    print("Tensorflow's version: " + tf.__version__)
    test_path = os.path.join('net', 'models')
    

if __name__=="__main__":
    main()
    