#include <iostream>
#include <vector>
#include <opencv2/core.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui.hpp>

#include <graph.h>

using namespace cv;
using namespace std;

int main(int argc, char **args) {

	int width;
	int height;
	cv::Mat img;

	img = cv::imread("D:\\documents\\davu_avatar.jpg", cv::IMREAD_COLOR);

	width = img.size().width;
	height = img.size().height;

	// cv::namedWindow( "Display window", cv::WINDOW_AUTOSIZE ); // Create a window for display.
 //    cv::imshow( "Display window", img );                // Show our image inside it.
 //    cv::waitKey(0); // Wait for a keystroke in the window
	std::cout << "Image size: (" << width << ", " << height << ")\n";
	std::cout << "    Chanel: " << img.channels() << std::endl;
	std::cout << "    Depth: " << img.depth() << std::endl;
	std::cout << "    Element size in byte: " << img.elemSize() << std::endl;

	unsigned char temp;
	Graph g;
	g.build();

	std:cout << g;

	return 0;
}