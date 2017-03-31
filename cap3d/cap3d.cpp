#include <iostream>
#include <fstream>
#include <exception>
#include <string>
#include <cstring>
#include <sstream>
#include <gm.hpp>
#include <camera.h>

#include <plyfile.hpp>
#include <glrenderer.h>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <opencv2/opencv.hpp>
#include <opencv2/core/core.hpp>

#define RAD(x) (x*3.14159f/180.0f)
#define BYTES_PER_PROP 4

#define OK 0

using namespace std;
using namespace gm;
using namespace fp;

typedef struct tagModel {
	int stride;
	char hasColor;
	int vertices_count;
	int face_count;
	int property_count;
	int *faces;
	float *vertices;

	tagModel() {
		property_count = 3;
		faces = 0;
		vertices = 0;
	}
} TModel;

// unsigned char* readObject(const char* filename, int &size);
// int readPly(const char* filename, TModel *model, BBox3d *bbox);
//
// void moveCameraTo(float ex, float ey, float ez, float cx, float cy, float cz, const TModel *model);
//
// void exportImage(const TModel* model);

int main(int argc, char* args[]) {

	if(argc < 2) {
		cout << "Not enough parameters. \n";
		cout << "USAGE: cap3d <input>.ply OR cap3d <input>.im \n";
		return 1;
	}

	GLFWwindow *window;
	if(!glfwInit()) {
		cout << "Failed to initialize GLFW!!" << endl;
		return -1;
	}

	window = glfwCreateWindow(640, 480, "Simple example", NULL, NULL);
	if (!window) {
		glfwTerminate();
		exit(EXIT_FAILURE);
	} //Failed to create window

	//Make our window current
	glfwMakeContextCurrent(window);

	//MUST initialize glew
	GLenum err = glewInit();
	if (err != GLEW_OK) {
		cout << "Initialize glew failed!!!!! \n";
		return 1;
	}

	// Ensure we can capture the escape key being pressed below
	glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);

	// Dark blue background
	glClearColor(0.0f, 0.0f, 0.4f, 0.0f);

	//Do our rendering here
	GLRenderer renderer;
	PlyFile f;

	//load model
	if(f.load(args[1]) != OK) {
		cout << args << endl;
		glfwTerminate();
		return 1;
	}

	do{
		// Clear the screen. It's not mentioned before Tutorial 02, but it can cause flickering, so it's there nonetheless.
		glClear( GL_COLOR_BUFFER_BIT );

		// Draw nothing, see you in tutorial 2 !
		//renderer.initGL();

		// Swap buffers
		glfwSwapBuffers(window);
		glfwPollEvents();

 // Check if the ESC key was pressed or the window was closed
	}while( glfwGetKey(window, GLFW_KEY_ESCAPE ) != GLFW_PRESS &&
		   glfwWindowShouldClose(window) == 0 );

	// Close OpenGL window and terminate GLFW
	glfwTerminate();
	return 0;
}

/*
void exportImage(const char* filename){
	BBox3d bbox;
	TModel model;

	readPly(filename, &model, &bbox);

	if(model.vertices != 0) {

		cout << "Min x: " << bbox.minx << "; Max x: " << bbox.maxx << std::endl;
		cout << "Min y: " << bbox.miny << "; Max y: " << bbox.maxy << std::endl;
		cout << "Min z: " << bbox.minz << "; Max z: " << bbox.maxz << std::endl;

		moveCameraTo(1.0f, 0.0f, -1.0f, 0.5f, 0.5f, 0.5f, &model);

		delete[] model.vertices;
	}
}

unsigned char* readObject(const char* filename, int &size) {
	unsigned char *data;
	ifstream f(filename, ios::binary);

	if(f.fail()) {
		return 0;
	}

	f.close();

	return data;
}

void moveCameraTo(float ex, float ey, float ez, float cx, float cy, float cz, const TModel *model){
	Camera cam;

	cam.setViewport(0, 0, 200, 200);
	cam.lookAt(ex, ey, ez, cx, cy, cz, 0.0f, 1.0f, 0.0f);
	float fov = RAD(45.0f) ;
	cam.setPerspective(fov, 0.1f, 99.0f);

	cv::Vec3b color;
	color[0] = 0;
	color[0] = 0;
	color[0] = 255;

	cv::Mat img(200, 200, CV_8UC3, cv::Scalar(0, 0, 0));
	float *out = new float[model->property_count * model->vertices_count];
	float p[3];
	int i, offs;
	int x, y;
	for(i=0; i<model->vertices_count; i++) {
		offs = i * model->property_count;
		cam.project(out + offs, model->vertices + offs);
		//cout << "x= " << out[offs] << "; y= " << out[offs + 1] << std::endl;

		x = (int) out[offs];
		y = (int) out[offs + 1];
		img.at<cv::Vec3b>(x, y) = color;
	}


	//write image matrice to file
	imwrite("pose2.jpg", img);



	//release memories
	delete[] out;
}
*/
