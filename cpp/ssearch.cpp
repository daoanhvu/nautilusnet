#include <iostream>
#include <opencv2/ximgproc/segmentation.hpp>
#include <opencv2/core/core.hpp>
#include <opencv2/imgcodecs/imgcodecs.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <ctime>

void help() {
	std::cout << "Usage: " << std::endl;
	std::cout << "    ssearch <filename> <option>" << std::endl;
	std::cout << "option:" << std::endl;
	std::cout << "    f: Fast search" << std::endl;
	std::cout << "    q: Quality search" << std::endl;
}

int main(int argc, char** args) {
	
	if(argc < 3) {
		help();
		return -1;
	}

	// speed up using multithreads
	cv::setUseOptimized(true);
	cv::setNumThreads(2);

	//read image
	cv::Mat img = cv::imread(args[1], CV_LOAD_IMAGE_COLOR /*CV_LOAD_IMAGE_UNCHANGED*/ );

	if(img.empty()) {
		std::cout << "[ERROR] Could not load image! " << std::endl;		
		return -2;
	}

	//resize image
	int newHeight = 200;
	int newWidth = (int)((1.0f * newHeight * img.cols) / img.rows);

	std::cout << "Height: " << img.rows << " -> " << newHeight << std::endl;
	std::cout << "Width: " << img.cols << " -> " << newWidth << std::endl;

	cv::Mat img2;

	cv::resize(img, img2, cv::Size(newWidth, newHeight), 0.0, 0.0, cv::INTER_LINEAR);

	// Create Selective search segmentation
	cv::Ptr<cv::ximgproc::segmentation::SelectiveSearchSegmentation> ss = cv::ximgproc::segmentation::createSelectiveSearchSegmentation();

	//set input image
	ss->setBaseImage(img2);

	switch(args[2][0]) {
		case 'f':
			ss->switchToSelectiveSearchFast();
		break;

		case 'q':
			ss->switchToSelectiveSearchQuality();
		break;

		default:
			help();
			return -3;
		
	}

	//now we run selective search
	std::vector<cv::Rect> rects;
	ss->process(rects);

	std::cout << "[INFO] Number of Region proposals: " << rects.size() << std::endl;

	int numShowRects = 100;
	int increment = 50;
	int k;
	bool shouldStop = false;

	while(!shouldStop) {
		cv::Mat imgOut = img2.clone();

		//iterate over all the region proposals
		// if the number of region is greater than numShowRects then break
		for(int i=0; (i < rects.size()) && (i < numShowRects); ++i) {
			cv::rectangle(imgOut, rects[i], cv::Scalar(0, 255, 255));
		}

		cv::namedWindow("Output", cv::WINDOW_NORMAL /* CV_WINDOW_AUTOSIZE*/);
		cv::imshow("Output", imgOut);

		k = cv::waitKey(10);

		switch(k) {
			case 109: //m
				numShowRects += increment;
			break;

			case 108: //l
				if(numShowRects > increment) {
					numShowRects -= increment;
				}
			break;

			case 113: //q
				shouldStop = true;
			break;
		}
	}

	return 0;
}