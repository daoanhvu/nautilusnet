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
#include <boost/algorithm/string.hpp>

using namespace std;
using namespace cv;
using namespace cv::cuda;

const size_t ERROR_IN_COMMAND_LINE = 1;

int create_dataset_text_file(const string &classes_file_path, const string &in_file_path, const string &out_file_path) {
	int l, space_idx, cls;
	string inImg;
	int num_of_file = 0;

	vector<string> classes;
	string line;
	std::ifstream classes_file(classes_file_path, std::ios::binary);
	int nClasses = 0;
	while(!classes_file.eof()) {
		std::getline(classes_file, line);
		boost::trim(line);
		if(line.length() > 0) {
			cout << line << " " << nClasses << endl;
			classes.push_back(line);
			nClasses++;
		}
	}
	classes_file.close();

	std::ifstream in_file(in_file_path, std::ios::binary);
	std::ofstream out_file(out_file_path);
	std::size_t pos;

	while(!in_file.eof()) {
		std::getline(in_file, inImg);
		l = inImg.length();

		if( l <= 0 )
			continue;

		space_idx = inImg.find_first_of(' ');
		if(space_idx > 0) {
			inImg = inImg.substr(0, space_idx);
		}
		
		cls = 0;
		while( cls < nClasses ) {
			pos = inImg.find(classes[cls]);
			//printf("%zu\n", pos);
			//cout << inImg << " " << classes[cls]  << endl;
			if(pos != std::string::npos ) {

				if( (inImg.find( "_001.png") != std::string::npos) || 
					(inImg.find( "_002.png") != std::string::npos) ||
						(inImg.find( "_003.png") != std::string::npos) ) {

					out_file << inImg << " " << cls << std::endl;
					num_of_file++;
					break;

				}
			}
			cls++;
		}
	}

	in_file.close();
	out_file.close();

	return num_of_file;

}

void resize_cpu(std::string inImg, std::string outImg, int newWidth, int newHeight);
void resize_gpu(std::string inImg, std::string outImg, int newWidth, int newHeight);

void get_output_path(std::string inImg, std::string output_folder, std::string &outpath);

int resize_images(const string &input_text_file, 
			const string &outputFolder, const string &image_dir,
			int width, int height);


int main(int argc, char* args[]) {
	boost::program_options::options_description desc("Options");
	desc.add_options() 
      ("help", "Print help messages")
      ("resize", "Request to resize images")
      ("subds", "Request to create sub-dataset")
      ("cls", boost::program_options::value<std::string>(),"classes file path")
      ("input_file", boost::program_options::value<std::string>(),"text file contains images path line by line")
      ("output_dir", boost::program_options::value<std::string>(),"output directory")
      ("image_dir", boost::program_options::value<std::string>(), "images folder")
      ("width", boost::program_options::value<int>(), "new width") 
      ("height", boost::program_options::value<int>(), "new height");

    boost::program_options::variables_map vm;
    try {
		boost::program_options::store(boost::program_options::parse_command_line(argc, args, desc), vm);

		if(vm.count("help")) {

		}

		boost::program_options::notify(vm);

		string input_text_file, outputFolder;

		if(vm.count("resize")) {
			input_text_file = vm["input_file"].as<std::string>();

	    	int width = vm["width"].as<int>();
	    	int height = vm["height"].as<int>();
	    
	    	string image_dir = "";
		    if(vm.count("image_dir")) {
		    	image_dir = vm["image_dir"].as<std::string>();
		    }

			outputFolder = vm["output_dir"].as<std::string>();

			resize_images(input_text_file, 
				outputFolder, image_dir, width, height);
		}

		if(vm.count("subds")) {
			string classes_file_path = vm["cls"].as<std::string>();
			input_text_file = vm["input_file"].as<std::string>();
			outputFolder = vm["output_dir"].as<std::string>();
			int num_image = create_dataset_text_file(classes_file_path, 
						input_text_file, outputFolder);

			cout << "Num of image in sub-dataset: " << num_image << endl;
		}

	} catch(boost::program_options::error &e) {
		std::cerr << "ERROR: " << e.what() << std::endl << std::endl; 
    	std::cerr << desc << std::endl; 
    	return ERROR_IN_COMMAND_LINE;
	}

	return 0;
}

int resize_images(const string &input_text_file, 
			const string &outputFolder, const string &image_dir,
			int width, int height) {

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

	text_file.close();

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