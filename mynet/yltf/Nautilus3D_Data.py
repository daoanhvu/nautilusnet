# Dao Anh Vu
# Dec 15, 2016
# For Training 3D YOLO

import math
import numpy as np
from scipy.misc import imread, imresize

from YOLO_PlottingUtils import *
from YOLO_CoverageMap import *
from preprocess_data import *
###### HYPERPARAMETERS #######
voc_data_path = 'home/davu/projects/nautilusnet/3Ddevkit/'
PERCENT_TRAIN_SET = 0.8
PERCENT_VAL_SET = 0.1
PERCENT_TEST_SET = 0.1
S = 7
B = 2.0
CLASSES = ["teddy", "ant", "airplane"]
NUM_CLASSES = len(CLASSES) # 20 for VOC, later will change for MSCOCO, ImageNet, etc.
classnameToIdxDict = {}
for i,classname in enumerate(CLASSES):
	classnameToIdxDict[classname] = i

### FOR DECIDING WHICH BBOXES CORRESPOND TO WHICH GRID CELLS ##########
NO_IMAGE_FLAG = 0 # COULD MAKE THIS -9999, ETC.
CONTAINS_IMAGE_FLAG = 1
#######################################################################


class BBox:
	def __init__(self, x, y, w, h, _cls):
		self.left = x
		self.top = y
		self.right = x + w
		self.bottom = y + h
		self.cls = _cls

class Image:
	def __init__(self, imgpath):
		self.path = imgpath
		self.bboxes = [];


def getData(textdatapath):
	"""
	INPUTS:
	-	N/A
	OUTPUTS:
	-	annotatedImages: Python list of annotated_image class objects
	##################################################################
	We retrieve data of the following format:
	class annotated_image:
	    def __init__(self, image_path):
	        self.image_path = image_path
	        # list of class bounding boxes
	        self.bounding_boxes = []
	class bounding_box:
	    def __init__(self, x_min, y_min, w, h, category):
	        self.x_min = x_min
	        self.y_min = y_min
	        self.w = w
	        self.h = h
	        self.category = category
	"""
	annotatedImages = []

	# filling the record_list
	input_file = open(textdatapath, 'r')
	for line in input_file:
		line = line.strip()
		ss = line.split(' ')
		img = Image(ss[0])
		bbs = [float(num) for num in ss[1:]]
		num_bbs = len(bbs) // 5
		if (len(bbs) % 5) != 0:
			print("Data error!")
			return None

		for i in range(num_bbs):
			x = bbs[i*5]
			y = bbs[i*5 + 1]
			w = bbs[i*5 + 2]
			h = bbs[i*5 + 3]
			c = bbs[i*5 + 4]
			bb = BBox(x, y, w, h, c)
			img.bboxes.append(bb)

		annotatedImages.append(img)

	return annotatedImages



#for testing

def main():
	data = getData('../../data/textdata.txt')
	print(data)

if __name__=='__main__':
	main()
