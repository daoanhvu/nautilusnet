# Dao Anh Vu
# Dec 15, 2016
# For Training 3D YOLO

import math
import numpy as np
from scipy.misc import imread, imresize

###### HYPERPARAMETERS #######
voc_data_path = 'home/davu/projects/nautilusnet/3Ddevkit/'
PERCENT_TRAIN_SET = 0.8
PERCENT_VAL_SET = 0.1
PERCENT_TEST_SET = 0.1
S = 7
B = 2
CLASSES = ["teddy", "ant", "airplane"]
NUM_CLASSES = len(CLASSES)
classnameToIdxDict = {}
for i,classname in enumerate(CLASSES):
	classnameToIdxDict[classname] = i

### FOR DECIDING WHICH BBOXES CORRESPOND TO WHICH GRID CELLS ##########
NO_IMAGE_FLAG = 0 # COULD MAKE THIS -9999, ETC.
CONTAINS_IMAGE_FLAG = 1
#######################################################################


class BBox:
	def __init__(self, x, y, w, h, _cls):
		self.x_min = x
		self.y_min = y
		self.w = w
		self.h = h
		self.category = _cls

class Image:
	def __init__(self, imgpath):
		self.image_path = imgpath
		self.bounding_boxes = [];


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
		if len(line) > 0:
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
				img.bounding_boxes.append(bb)

			annotatedImages.append(img)

	return annotatedImages
def computeCoverageMapSimplified(im,gtBbox):
	"""
	INPUTS:
	-	im: n-d array containing resized [448,448,3] image
	-	gtBbox: (4x1) n-d array containing bounding box info [xmin,ymin,xmax,ymax]
	OUTPUTS:
	-	coverageMap: (7x7) n-d array containing pr(object)=1 if portion of obj lies in that grid cell
	"""
	GRID_SIZE = im.shape[0] / float(S)

	xmin = gtBbox[0]
	ymin = gtBbox[1]
	xmax = gtBbox[2]
	ymax = gtBbox[3]

	leftGridEdge = (xmin - (xmin % GRID_SIZE))/ GRID_SIZE
	topGridEdge = (ymin - (ymin % GRID_SIZE))/ GRID_SIZE
	rightGridEdge = (xmax + ( (448-xmax) % GRID_SIZE) )/GRID_SIZE
	botGridEdge = (ymax + ( (448-ymax) % GRID_SIZE) )/GRID_SIZE

	leftGridEdge = max( leftGridEdge, 0 )
	topGridEdge = max( topGridEdge, 0 )
	rightGridEdge = min( rightGridEdge, 7 )
	botGridEdge = min( botGridEdge, 7 )

	coverageMap = np.zeros((7,7))
	for col in range(7):
		for row in range(7):
			if (col >= leftGridEdge) and (col<rightGridEdge):
				if (row >= topGridEdge) and (row<botGridEdge):
					coverageMap[row,col] = 1

	return coverageMap

def computeCoverageMap(im, bbox):
	"""
	I use xmin,ymin,xmax,ymax to find an object coverage map for each ground truth box.
	INPUTS:
	-	im: n-d array, NOT-yet-resized-to-448-448-3 image
	-	bbox:
	OUTPUTS:
	-	coverageMap: n-d array, 49x1
	"""
	im = im.astype(np.float32)
	gtBbox = np.array([bbox.x_min,bbox.y_min,bbox.x_min+bbox.w,bbox.y_min+bbox.h])
	xScale = 448.0 / im.shape[1]
	yScale = 448.0 / im.shape[0]
	gtBbox[0] *= xScale
	gtBbox[2] *= xScale
	gtBbox[1] *= yScale
	gtBbox[3] *= yScale
	im = imresize(im, [448,448])
	coverageMap = computeCoverageMapSimplified(im,gtBbox)
	coverageMap = np.reshape( coverageMap, [-1])
	return coverageMap

def get_batch(annotatedImages, bs):
	"""
	If center of a bounding box falls into a grid cell, that grid cell is
	responsible for detecting that bounding box. So I store that bbox info
	for that particular grid cell.
	INPUTS:
	-	annotatedImages: a Python list of annotated_image class objects
	OUTPUTS:
	-	minibatchIms: n-d array
	-	minibatchGTs: n-d array, shape [num_gtbox, 56=NUM_CLASSES+4+49] (gt labels)
	"""
	# print '=====> Sampling Ground Truth tensors for %d images ====>' % (len(annotatedImages))

	batch_size = bs
	mask = np.random.choice( len(annotatedImages) , batch_size ) # RANDOM SAMPLE OF THE INDICES
	imNum = mask[0]
	annotatedImage = annotatedImages[imNum]
	image_path = annotatedImage.image_path
	img = imread(image_path)
	img = imresize(img, (448, 448))
	img = img[...,::-1]
	# darknet scales color values from 0 to 1
	# https://github.com/pjreddie/darknet/blob/c6afc7ff1499fbbe64069e1843d7929bd7ae2eaa/src/image.c#L469
	img = (img / 255.0)

	# if imNum > ARBITRARY_STOP_LOADING_IMS_NUMBER_FOR_DEBUGGING:
	# 	break

	gt_classes = np.zeros((49,NUM_CLASSES))
	gt_conf = np.zeros((49,4))
	ind_obj_i = np.zeros((49))
	gt_boxes_j0 = np.zeros((49,4))

	im = imread(image_path)
	gt = []
	occupiedSlot = np.zeros((S,S,B))
	# We limit to two bounding boxes per grid cell.
	# For each image, tell which grid cells are responsible for which bboxes
	for i, bbox in enumerate(annotatedImage.bounding_boxes):
		x_cent = bbox.x_min + bbox.w / 2.
		y_cent = bbox.y_min + bbox.h / 2.
		normalizedXCent, normalizedYCent, gridCellRow, gridCellCol = normXYToGrid(x_cent,y_cent,im)
		normalizedW = bbox.w * 1.0 / im.shape[1] # dividing by im width
		normalizedH = bbox.h * 1.0 / im.shape[0] # dividing by im height
		gridCellRow = int(gridCellRow)
		gridCellCol = int(gridCellCol)
		# classIdx = classnameToIdxDict[ bbox.category ] # convert string to int
		classIdx = int(bbox.category)
		coverageMap = computeCoverageMap(im, bbox) # Returns 49x1 coverage map

  		# indicating if that grid cell contains any object
		ind_obj_i = np.logical_or( ind_obj_i, coverageMap).astype(np.int64)

		if occupiedSlot[gridCellRow,gridCellCol,0] == NO_IMAGE_FLAG:
			j = 0 # 2nd box slot for this grid cell
			gt_classes[gridCellRow * 7 + gridCellCol, classIdx ] = 1
			xywh = np.array([ normalizedXCent, normalizedYCent, math.sqrt(normalizedW), math.sqrt(normalizedH) ])
			bboxGT = xywh # coverage map is the confidence
			gt_boxes_j0[ gridCellRow * 7 + gridCellCol] = bboxGT
			occupiedSlot[gridCellRow,gridCellCol,j] = CONTAINS_IMAGE_FLAG

			# values in each of 4 columns are identical (tiled/repmatted). Object here at cell i!
			gt_conf[gridCellRow * 7 + gridCellCol,:] = np.ones((1,4))

  		# IGNORING J=1 IN SIMPLIFIED, CURRENT CASE
		# elif occupiedSlot[gridCellRow,gridCellCol,1] == NO_IMAGE_FLAG:
		# 	j = 1 # 2nd box slot for this grid cell
		# 	classGTs = np.zeros(NUM_CLASSES)
		# 	classGTs[classIdx] = 1
		# 	xywh = np.array([ normalizedXCent, normalizedYCent, math.sqrt(normalizedW), math.sqrt(normalizedH) ])
		# 	gt = np.hstack(( classGTs , xywh, coverageMap)) # coverage map is the confidence
		# 	occupiedSlot[gridCellRow,gridCellCol,j] = CONTAINS_IMAGE_FLAG
		else:
			#print 'In Image %d, no more room in some grid cell for this bbox.' % (imNum)
			pass

	minibatchIms = img
	# print("BOXES: ", gt_boxes_j0)
	return minibatchIms, (gt_conf,gt_classes,ind_obj_i,gt_boxes_j0)

def get_batch_data(annotatedImages, bs):
	batch_size = bs
	mask = np.random.choice( len(annotatedImages) , batch_size ) # RANDOM SAMPLE OF THE INDICES

	gt_classes = np.zeros((batch_size, 49,NUM_CLASSES))
	gt_conf = np.zeros((batch_size, 49,4))
	ind_obj_i = np.zeros((batch_size, 49))
	gt_boxes_j0 = np.zeros((batch_size, 49,4))
	imgs = np.zeros((batch_size, 448, 448, 3))

	print("Batch size: ", batch_size)
	for k in range(batch_size):
		imNum = mask[k]
		print("Batch index: ", imNum)
		annotatedImage = annotatedImages[imNum]
		image_path = annotatedImage.image_path
		print("Loading image: ", image_path)
		img = imread(image_path)
		img = imresize(img, (448, 448))
		img = img[...,::-1] # shape of [448, 448, 3]
		# darknet scales color values from 0 to 1
		# https://github.com/pjreddie/darknet/blob/c6afc7ff1499fbbe64069e1843d7929bd7ae2eaa/src/image.c#L469
		img = (img / 255.0)
		im = imread(image_path)
		gt = []
		occupiedSlot = np.zeros((S,S,B))
		# We limit to two bounding boxes per grid cell.
		# For each image, tell which grid cells are responsible for which bboxes
		for i, bbox in enumerate(annotatedImage.bounding_boxes):
			x_cent = bbox.x_min + bbox.w / 2.
			y_cent = bbox.y_min + bbox.h / 2.
			normalizedXCent, normalizedYCent, gridCellRow, gridCellCol = normXYToGrid(x_cent,y_cent,im)
			normalizedW = bbox.w * 1.0 / im.shape[1] # dividing by im width
			normalizedH = bbox.h * 1.0 / im.shape[0] # dividing by im height
			gridCellRow = int(gridCellRow)
			gridCellCol = int(gridCellCol)
			# classIdx = classnameToIdxDict[ bbox.category ] # convert string to int
			classIdx = int(bbox.category)
			coverageMap = computeCoverageMap(im, bbox) # Returns 49x1 coverage map

	  		# indicating if that grid cell contains any object
			ind_obj_i[k, :] = np.logical_or(ind_obj_i[k, :], coverageMap).astype(np.int64)

			if occupiedSlot[gridCellRow,gridCellCol,0] == NO_IMAGE_FLAG:
				j = 0 # 2nd box slot for this grid cell
				gt_classes[k, gridCellRow * 7 + gridCellCol, classIdx ] = 1
				xywh = np.array([ normalizedXCent, normalizedYCent, math.sqrt(normalizedW), math.sqrt(normalizedH) ])
				bboxGT = xywh # coverage map is the confidence
				gt_boxes_j0[k, gridCellRow * 7 + gridCellCol, :] = bboxGT
				occupiedSlot[gridCellRow,gridCellCol,j] = CONTAINS_IMAGE_FLAG

				# values in each of 4 columns are identical (tiled/repmatted). Object here at cell i!
				gt_conf[k, gridCellRow * 7 + gridCellCol,:] = np.ones((1,4))

			imgs[k, : ] = img

		return imgs, (gt_conf,gt_classes,ind_obj_i,gt_boxes_j0)

def normXYToGrid(x_cent,y_cent,im):
	"""
	I normalize the x,y coordinates to be the offset from top-left grid corner,
	normalized to size of grid.
	The (x, y) coordinates represent the center of the box relative to the
	bounds of the grid cell.
	The width and height are predicted relative to the whole image.
	In contrast, w and h from the GTs are normalized to the image.
	INPUTS:
	-	x_cent: float, x-coordinate of center of a bounding box
	-	y_cent: float, y-coordinate of center of a bounding box
	-	im: n-d array, representing an image of shape [None,None,3]
	OUTPUTS:
	-	normalizedXCent: float, x-coordinate of center of bbox, normalized to grid cell size
	-	normalizedYCent: float, y-coordinate of center of bbox, normalized to grid cell size
	-	gridCellRow: int, index of row in which we find the bbox center
	-	gridCellCol: int, index of column in which we find the bbox center
	"""
	gridCellWidth = im.shape[1] / 7.
	gridCellHeight = im.shape[0] / 7.
	gridCellRow = math.floor( y_cent / gridCellHeight )
	gridCellCol = math.floor( x_cent / gridCellWidth )
	normalizedXCent=(x_cent-(gridCellCol * gridCellWidth))/gridCellWidth
	normalizedYCent=(y_cent-(gridCellRow * gridCellHeight))/gridCellHeight
	return normalizedXCent, normalizedYCent, gridCellRow, gridCellCol

def separateDataSets(annotatedImages):
	"""
	INPUTS:
	-	annotatedImages: python list of annotated_image class objects
	OUTPUTS:
	-	trainData: python list of annotated_image class objects
	-	valData: python list of annotated_image class objects
	-	testData: python list of annotated_image class objects
	"""
	trainNum = int( math.floor( PERCENT_TRAIN_SET * len(annotatedImages) ) )
	valNum = int( math.floor( PERCENT_VAL_SET * len(annotatedImages) ) )
	testNum = int( len(annotatedImages) - trainNum - valNum )
	print('===> Placing %f %% into training set, %f %% into val set, %f %% into test set' % (PERCENT_TRAIN_SET,PERCENT_VAL_SET,PERCENT_TEST_SET))
	print('===> %d ims in training set, %d ims in val set, %d ims in test set' % (trainNum,valNum,testNum))
	trainEndIdx = trainNum
	valEndIdx = trainNum + valNum
	testIndIdx = trainNum + valNum + testNum
	trainData = annotatedImages[0:trainEndIdx]
	valData = annotatedImages[trainEndIdx:valEndIdx]
	testData = annotatedImages[valEndIdx:testIndIdx]
	return trainData, valData, testData

#for testing
def main():
	mask = np.random.choice( 2 , 5 ) # RANDOM SAMPLE OF THE INDICES
	for k in range(5):
		print(mask[k])

if __name__=='__main__':
	main()
