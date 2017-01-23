#ifndef _OPENCVIMAGEHANDLER_H
#define _OPENCVIMAGEHANDLER_H

#include <opencv2/core.hpp>
//#include "D:\programs\opencv\build\include\opencv2\core\core.hpp"

class OpenCVImageHandler {
	public:
		static bool readImage(HWND hwnd, const char *filename, cv::Mat mat);
};
#endif