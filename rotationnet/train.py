import os
import sys
import caffe
import numpy as np
import argparse
import glob
import time


CAFFE_ROOT = "/Users/davu/framework/caffe/"


def main(argv):
    parser = argparse.ArgumentParser()
    parser.add_argument("--input_file", help="Input Image directory")
    parser.add_argument(
        "--output_file",
        help="Output npy filename."
    )
    # Optional arguments.
    parser.add_argument(
        "--model_def",
        default="deploy_nautilus_3.prototxt",
        help="Model definition file")
    parser.add_argument(
        "--pretrained_model",
        default="/Volumes/data/projects/nautilusnet/rotationnet/models/nautilus_iter_40.caffemodel",
        help="Trained model weights file."
    )
    parser.add_argument(
        "--mean_file",
        default='/Volumes/data/projects/nautilusnet/rotationnet/nautilus_mean.npy',
        help="Data set image mean of H x W x K dimensions (numpy array). " +
             "Set to '' for no mean subtraction."
    )
    parser.add_argument(
        "--input_scale",
        type=float,
        default=255,
        help="Multiply input features by this scale before input to net"
    )
    parser.add_argument(
        "--images_dim",
        default='448,448',
        help="Canonical 'height,width' dimensions of input images."
    )
    parser.add_argument(
        "--channel_swap",
        default='2,1,0',
        help="Order to permute input channels. The default converts " +
             "RGB -> BGR since BGR is the Caffe default by way of OpenCV."
    )
    parser.add_argument(
        "--center_only",
        action='store_true',
        help="Switch for prediction from center crop alone instead of " +
             "averaging predictions across crops (default)."
    )
    parser.add_argument(
        "--gpu",
        action='store_true',
        help="Switch for gpu computation."
    )

    args = parser.parse_args()
    print(args)

    image_dims = [int(s) for s in args.images_dim.split(',')]
    channel_swap = [int(s) for s in args.channel_swap.split(',')]

    mean = None
    if args.mean_file:
        mean = np.load(args.mean_file)
        # Resize mean (which requires H x W x K input in range [0,1]).
        in_shape = (227, 227)
        m_min, m_max = mean.min(), mean.max()
        normal_mean = (mean - m_min) / (m_max - m_min)
        mean = caffe.io.resize_image(normal_mean.transpose((1,2,0)),
                                     in_shape).transpose((2,0,1)) * (m_max - m_min) + m_min

	if args.gpu:
		caffe.set_mode_gpu()
	else:
		caffe.set_mode_cpu()

	net = caffe.Classifier(args.model_def, args.pretrained_model, image_dims=image_dims, mean=mean,
		input_scale=1.0, raw_scale=255.0, channel_swap=channel_swap)

    # Load image file.
    args.input_file = os.path.expanduser(args.input_file)
    f = open(args.input_file)
    im_files_ = f.readlines()
    im_files = []
    for i in range(len(im_files_)):
        im_f = im_files_[ i ].split( ' ' )
        if len(im_f) == 1:
            im_f[ 0 ] = im_f[ 0 ][:-1]
        im_files.append(im_f[ 0 ])

    inputs =[caffe.io.load_image(im_f) for im_f in im_files]

    print "Classifying %d inputs." % len(inputs)

    # Classify.
    start = time.time()
    predictions = net.predict(inputs, not args.center_only)
    print "Done in %.2f s." % (time.time() - start)

    # Save
    np.save(args.output_file, predictions)
    print "Saved %s." % args.output_file

if __name__=='__main__':
	main(sys.argv)