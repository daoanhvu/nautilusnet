#ifndef _OPENCVIMAGEHANDLER_H
#define _OPENCVIMAGEHANDLER_H

#include <opencv2/core.hpp>

class OpenCVImageHandler {
	public:
		static bool readImage(HWND hwnd, const char *filename, cv::Mat mat);
};
#endif