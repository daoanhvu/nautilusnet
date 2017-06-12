import os
import sys
import caffe
import argparse

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
        default="deploy_nautilusnet.prototxt",
        help="Model definition file."
    )
    parser.add_argument(
        "--mean_file",
        default='nautilus_mean_file.binaryproto',
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

	args = parser.parse_args()
	print(args)

	mean = None
	if args.mean_file:
		mean = np.load(args.mean_file)

	if args.gpu:
		caffe.set_mode_gpu()
	else:
		caffe.set_mode_cpu()

	net = cf.Classifier(args.model_def, args.pretrained_model, image_dims=image_dims, mean=mean,
		input_scale=1.0, raw_scale=255.0, channel_swap=channel_swap)

if __name__=='__main__':
	main(sys.argv)