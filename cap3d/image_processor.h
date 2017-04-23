#ifndef _IMAGE_PROCESSOR_H_
#define _IMAGE_PROCESSOR_H_

#include <opencv2/opencv.hpp>
#include <opencv2/core/core.hpp>

int detectBoundingBox(cv::Mat img, cv::Vec3b bgcolor, cv::Rect &bbox);

#endif
