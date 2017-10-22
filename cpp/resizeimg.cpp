#include <iostream>
#include <fstream>
#include <string>
#include <vector>

using namespace std;

#include <opencv2/opencv.hpp>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

#ifdef GPU
	#include <opencv2/core/cuda.hpp>
	#include <opencv2/cudawarping.hpp>
#endif

#include <boost/filesystem.hpp>
#include <boost/program_options.hpp>

using namespace std;
using namespace cv;
using namespace cv::cuda;

const size_t ERROR_IN_COMMAND_LINE = 1;

void resize_cpu(std::string inImg, std::string outImg, int newWidth, int newHeight);
void resize_gpu(std::string inImg, std::string outImg, int newWidth, int newHeight);

void get_output_path(std::string inImg, std::string output_folder, std::string &outpath);


int main(int argc, char* args[]) {

	boost::program_options::options_description desc("Options");
	desc.add_options() 
      ("help", "Print help messages")
      ("input_file", boost::program_options::value<std::string>()->required(),"text file contains images path line by line")
      ("output_dir", boost::program_options::value<std::string>()->required(),"output directory")
      ("image_dir", boost::program_options::value<std::string>(), "images folder")
      ("width", boost::program_options::value<int>()->required(), "new width") 
      ("height", boost::program_options::value<int>()->required(), "new height");

	std::string input_text_file;
	std::string image_dir = "";
	int width = 0;
	int height = 0;
	std::string outputFolder;
	boost::program_options::variables_map vm;
	try {
		boost::program_options::store(boost::program_options::parse_command_line(argc, args, desc), vm);

		if(vm.count("help")) {

		}

		boost::program_options::notify(vm);
    
	    input_text_file = vm["input_file"].as<std::string>();

	    width = vm["width"].as<int>();
	    height = vm["height"].as<int>();
	    
	    if(vm.count("image_dir")) {
	    	image_dir = vm["image_dir"].as<std::string>();
	    }

		outputFolder = vm["output_dir"].as<std::string>();

	} catch(boost::program_options::error &e) {
		std::cerr << "ERROR: " << e.what() << std::endl << std::endl; 
    	std::cerr << desc << std::endl; 
    	return ERROR_IN_COMMAND_LINE;
	}

	std::ifstream text_file(input_text_file, std::ios::binary);
	std::string outString;

	int l;
	int space_idx;
	std::string inImg;
	std::string outFullFolder;
	boost::filesystem::path outPath;

	cout << "Going to resize to size(" << width << ", " << height << ")" << endl;

	while(!text_file.eof()) {
		std::getline(text_file, inImg);
		l = inImg.length();

		if( l <= 0 )
			continue;

		space_idx = inImg.find_first_of(' ');
		if(space_idx > 0) {
			inImg = inImg.substr(0, space_idx);
		}
		std::string in_img_path = image_dir + inImg;

		get_output_path(inImg, outputFolder, outString);

		int last_plash = outString.find_last_of('/');
		outFullFolder = outString.substr(0, last_plash);

		outPath = boost::filesystem::system_complete(outFullFolder);
		boost::filesystem::create_directories(outPath);
		resize_cpu(in_img_path, outString, width, height);

	}

	return 0;
}

#ifdef GPU
void resize_cpu(std::string input_file, std::string output_file, int newWidth, int newHeight) {
	//try to use OpenCV CUDA
	//Read input image from the disk
	Mat inputCpu = imread(input_file,CV_LOAD_IMAGE_COLOR);
	GpuMat input (inputCpu);
	if(input.empty()) {
		std::cout<<"Image Not Found: "<< input_file << std::endl;
		return;
	}

	//Create output image
	GpuMat output;
	cv::Size newSize = cv::Size(newWidth, newHeight);

	//GpuTimer timer;
	//timer.Start();
	cuda::resize(input, output, newSize, 0.0, 0.0, CV_INTER_AREA); // downscale 4x

	//timer.Stop();

	Mat outputCpu;
	output.download(outputCpu);
	imwrite(output_file, outputCpu);

	input.release();
	output.release();

	// cout << "Write image " << output_file << endl;
}
#else
void resize_cpu(std::string input_file, std::string output_file, int newWidth, int newHeight) {
	//Read input image from the disk
	Mat inputCpu = imread(input_file, CV_LOAD_IMAGE_COLOR);
	if(!inputCpu.data ) {
		cout << "ERROR: Couldn't read from file " << input_file << endl;
		return;
	}
	cv::Size newSize = cv::Size(newWidth, newHeight);
	Mat outputCpu = cv::Mat(newSize, inputCpu.type());
	cv::resize(inputCpu, outputCpu, newSize, 0.0, 0.0, CV_INTER_AREA);
	imwrite(output_file, outputCpu);
}
#endif
void resize_gpu(std::string inImg, std::string outImg, int newWidth, int newHeight) {

}

void get_output_path(std::string inImg, std::string output_folder, std::string &outpath) {
	int first_plash = inImg.find_first_of('/');
	// int last_plash = inImg.find_last_of('/');
	if(first_plash < 0) {
		outpath = output_folder + "/" + inImg;
	} else {
		outpath = inImg;
		outpath.replace(0, first_plash+1, output_folder + "/");
	}
}