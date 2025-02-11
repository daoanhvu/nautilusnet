import torch
import cv2
import threading
import numpy as np
from PIL import ImageGrab


def run(model, im):
    results = model(im)
    detections = results.pandas().xyxy[0]
    person_number = 0
    for index, row in detections.iterrows():
        x1, y1, x2, y2, confidence, class_id, name = int(row['xmin']), int(row['ymin']), int(
            row['xmax']), int(row['ymax']), row['confidence'], row['class'], row['name']
        if name == 'person':
            person_number += 1
        # Draw the bounding box
        cv2.rectangle(image, (x1, y1), (x2, y2), (255, 0, 0), 2)

        # Draw the label
        label = f"{name} {confidence:.2f}"
        cv2.putText(image, label, (x1, y1 - 10),
                    cv2.FONT_HERSHEY_SIMPLEX, 0.5, (255, 0, 0), 2)

    print('Number of people in scene: ', person_number)
    # Display the image
    cv2.imshow('Detections', image)
    cv2.waitKey(0)
    cv2.destroyAllWindows()


if __name__ == '__main__':
    model = torch.hub.load("ultralytics/yolov5", "yolov5s", device=0)
    image_path = 'D:\\projects\\beer-vision\\test_data\\test_img1.jpg'
    image = cv2.imread(image_path)
    img_rgb = cv2.cvtColor(image, cv2.COLOR_BGR2RGB)

    # threading.Thread(target=run, args=[model, img_rgb], daemon=True).start()
    run(model=model, im=img_rgb)
