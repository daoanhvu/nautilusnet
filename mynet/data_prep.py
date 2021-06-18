from collections import namedtuple
import os
import pathlib
import argparse
import glob
import io
import tensorflow as tf
from PIL import Image
import pandas as pd
import xml.etree.ElementTree as ET
from object_detection.utils import dataset_util, label_map_util

def xml_to_csv(xml_folder):
    xml_list = []
    for xml in glob.glob(xml_folder + '/*.xml'):
        tree = ET.parse(xml)
        root = tree.getroot()
        file_path = root.find('path').text
        img_size = root.find('size')
        image_height = int(img_size.find('height').text)
        image_width = int(img_size.find('width').text)
        image_channels = int(img_size.find('depth').text)
        for member in root.findall('object'):
            bndbox = member.find('bndbox')
            cls_name = member.find('name')
            value = (file_path, image_width, image_height, image_channels, 
                int(bndbox.find('xmin').text),
                int(bndbox.find('ymin').text),
                int(bndbox.find('xmax').text),
                int(bndbox.find('ymax').text),
                cls_name)
            xml_list.append(value)
    column_names = ['filename', 'width', 'height', 'channels', 'xmin', 'ymin', 'xmax', 'ymax', 'class_name']
    return pd.DataFrame(xml_list, columns=column_names)

def split_examples(df, group):
    data = namedtuple('data', ['filename', 'object'])
    grouped_data = df.groupby(group)
    return [data(filename, grouped_data.get_group(x)) for filename , x in zip(grouped_data.groups.keys(), grouped_data.groups)]

def create_tf_example(group, path, label_map_dict):
    with tf.io.gfile.GFile(os.path.join(path, '{}'.format(group.filename)), 'rb') as fid:
        encoded_img = fid.read()
    
    encoded_img_io = io.BytesIO(encoded_img)
    image = Image.open(encoded_img_io)
    width, height = image.size
    filename = group.filename.encode('utf8')
    image_format = b'jpg'
    xmins = []
    xmaxs = []
    ymins = []
    ymaxs = []
    class_texts = []
    classes = []

    for index, row in group.object.iterrows():
        xmins.append(row['xmin'] / width)
        xmaxs.append(row['xmax'] / width)
        ymins.append(row['ymin'] / height)
        ymaxs.append(row['ymax'] / height)
        class_texts.append(row['class_name'].text.encode('utf8'))
        classes.append(label_map_dict[row['class_name'].text])

    return tf.train.Example(features=tf.train.Features(feature={
                'image/height': dataset_util.int64_feature(height),
                'image/width': dataset_util.int64_feature(width),
                'image/filename': dataset_util.bytes_feature(filename),
                'image/source_id': dataset_util.bytes_feature(filename),
                'image/encoded': dataset_util.bytes_feature(encoded_img),
                'image/format': dataset_util.bytes_feature(image_format),
                'image/object/bbox/xmin': dataset_util.float_list_feature(xmins),
                'image/object/bbox/xmax': dataset_util.float_list_feature(xmaxs),
                'image/object/bbox/ymin': dataset_util.float_list_feature(ymins),
                'image/object/bbox/ymax': dataset_util.float_list_feature(ymaxs),
                'image/object/class/text': dataset_util.bytes_list_feature(class_texts),
                'image/object/class/label': dataset_util.int64_list_feature(classes),
            }))
    


# python3 --i /Users/vdao/projects/ml/Faster-R-CNN-main/data/xml \
# --lp /Users/vdao/projects/ml/Faster-R-CNN-main/data/labelMap_duck_chicken.pbtxt \
# --o /Users/vdao/projects/ml/Faster-R-CNN-main/data/duck_chicken.tfrecord
if __name__=='__main__':
    parser = argparse.ArgumentParser(
       description=
       'Reads all XML files, generated by labelImg, from a directory and generates a single CSV file',
       formatter_class=argparse.RawDescriptionHelpFormatter)

    parser.add_argument('--xml', type=str, required=True,
       help='Directory containing the XML files generated by labelImg or path to a single VIA JSON')

    parser.add_argument('--img', type=str, required=True,
       help='Directory containing the XML files generated by labelImg or path to a single VIA JSON')

    parser.add_argument('--lp', type=str, required=True,
       help='Path to label map file (*.pbtxt)')

    parser.add_argument('--o', type=str, required=True, help='Path to the TFRecord file')

    parser.add_argument('--csv', type=str, required=False, 
        help='In case you want to export examples to CSV, this is the path to the CSV file')

    args = parser.parse_args()
    print("Tensorflow's version: " + tf.__version__)
    image_path = os.path.join(args.img)
    examples = xml_to_csv(args.xml)
    grouped = split_examples(examples, 'filename')
    label_map = label_map_util.load_labelmap(args.lp)
    label_map_dict = label_map_util.get_label_map_dict(label_map)
    writer = tf.io.TFRecordWriter(args.o)

    for group in grouped:
        tf_example = create_tf_example(group, image_path, label_map_dict)
        writer.write(tf_example.SerializeToString())
    writer.close()

    print('Successfully created the TFRecord file: {}'.format(args.o))
    if args.csv is not None:
        examples.to_csv(args.csv, index=None)
        print('Successfully created the CSV file: {}'.format(args.csv))
