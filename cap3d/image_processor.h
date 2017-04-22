#ifndef _IMAGE_PROCESSOR_H_
#define _IMAGE_PROCESSOR_H_

#include <opencv2/opencv.hpp>
#include <opencv2/core/core.hpp>

typedef struct tagBBox3d {
  int left;
  int right;
  int top;
  int bottom;
} BBox2d;

int detectBoundingBox(cv::Mat img, cv::Vec3b bgcolor, BBox2d &bbox);

#endif
