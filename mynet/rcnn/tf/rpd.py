from tensorflow.keras.applications import ResNet50
from tensorflow.keras.applications.resnet50 import preprocess_input
from tensorflow.keras.applications import imagenet_utils
from tensorflow.keras.preprocessing.image import img_to_array
from imutils.object_detection import non_max_suppression
import numpy as np
import argparse
import cv2

def selective_search(image, method="fast"):
    ss = cv2.ximgproc.segmentation.createSelectiveSearchSegmentation()
    ss.setBaseImage(image)

    if method == "fast":
        ss.switchToSelectiveSearchFast()
    else:
        ss.switchToSelectiveSearchQuality()

    rects = ss.process()

    return rects

ap = argparse.ArgumentParser()
ap.add_argument("-i", "--image", required=True, help="Path to the input image")
ap.add_argument("-m", "--method", type=str, default="fast", choices=["fast", "quality"], help="selective search method")
ap.add_argument("-c", "--conf", type=float, default=0.9, help="minimum probability to consider a classification/detection")
ap.add_argument("-f", "--filter", type=str, default=None, help="comma separated list of ImageNet labels to filter on")
args = vars(ap.parse_args())

labelFilters = args["filter"]
if labelFilters is not None:
    labelFilters = labelFilters.lower().split(",")

print("[INFO] Loading ResNet...")
model = ResNet50(weights="imagenet")

image = cv2.imread(args["image"])
(H, W) = image.shape[:2]

print("[INFO] Performing selective search with '{}' method...".format(args["method"]))
rects = selective_search(image, method=args["method"])
print("[INFO] {} regions found by selective search".format(len(rects)))

proposals = []
boxes = []

for (x, y, w, h) in rects:
    if w/float(W) < 0.1 or h/float(H) < 0.1:
        continue

    roi = image[y:y + h, x:x + w]
    roi = cv2.cvtColor(roi, cv2.COLOR_BGR2RGB)
    roi = cv2.resize(roi, (224, 224))

    roi = img_to_array(roi)
    roi = preprocess_input(roi)

    proposals.append(roi)
    boxes.append((x, y, w, h))

proposals = np.array(proposals)
print("[INFO] Proposal shape: {}".format(proposals.shape))

print("[INFO] Classifying proposals...")
preds = model.predict(proposals)
preds = imagenet_utils.decode_predictions(preds, top=1)

labels = {}

for (i, p) in enumerate(preds):
    (imagenetID, label, prob) = p[0]

    if labelFilters is not None and label not in labelFilters:
        continue

    if prob >= args["conf"]:
        (x, y, w, h) = boxes[i]
        box = (x, y, x + w, y + h)

        L = labels.get(label, [])
        L.append((box, prob))
        labels[label] = L

for label in labels.keys():
    print("[INFO] Showing results for '{}'".format(label))
    clone = image.copy()

    for (box, prob) in labels[label]:
        (startX, startY, endX, endY) = box
        cv2.rectangle(clone, (startX, startY), (endX, endY), (0, 225, 0), 2)

    cv2.imshow("Before", clone)
    clone = image.copy()

    boxes = np.array([p[0] for p in labels[label]])
    proba = np.array([p[1] for p in labels[label]])
    boxes = non_max_suppression(boxes, proba)

    for (startX, startY, endX, endY) in boxes:
        cv2.rectangle(clone, (startX, startY), (endX, endY), (0, 225, 0), 2)
        y = startY - 10 if startY - 10 > 10 else startY + 10
        cv2.putText(clone, label, (startX, y),
			cv2.FONT_HERSHEY_SIMPLEX, 0.45, (0, 255, 0), 2)

        cv2.imshow("After", clone)
        cv2.waitKey(0)