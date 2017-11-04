#include <iostream>
#include <fstream>
#include <exception>
#include <string>
#include <cstring>
#include <sstream>
#include <vector>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <opencv2/opencv.hpp>
#include <opencv2/core/core.hpp>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glm/gtx/euler_angles.hpp>
#include <glm/gtx/norm.hpp>

#include "camera.h"
#include "vbo.h"
#include "viewer.h"
#include "reader/pcdreader.h"
#include "reader/pcdmodel3d.h"
#include "reader/plyreader.h"
#include "reader/plymodel3d.h"
#include "glrenderer.h"
#include "lexer_parser.h"
#include "shader.h"
#include "image_processor.h"
#include "utils.h"

#define PI 3.14159f
#define RAD(x) ( x * 3.14159265f / 180.0f )
#define BYTES_PER_PROP 4

#define OK 0

#define OS_UNKNOWN 0
#define OS_X 1
#define LINUX 2
#define WINDOWS 3


using namespace std;
using namespace glm;

#ifdef __APPLE__
	const int OS = OS_X;
#else
	#ifdef __linux__
		const int OS = LINUX;
	#else
		#ifdef _win32
			const int OS = WINDOWS;
		#else
			const int OS = OS_UNKNOWN;
		#endif
	#endif
#endif


const GLuint POSITION_LOCATION = 0;
const GLuint NORMAL_LOCATION = 1;
const GLuint COLOR_LOCATION = 2;

glm::mat4 viewMatrix;
glm::mat4 projectionMatrix;

// Initial position : on +Z
glm::vec3 position = glm::vec3( 0, 0, 9 );
// Initial horizontal angle : toward -Z
float horizontalAngle = 3.14f;
// Initial vertical angle : none
float verticalAngle = 0.0f;
// Initial Field of View
float initialFoV = 45.0f;

float speed = 3.0f; // 3 units / second
float mouseSpeed = 0.005f;

//const std::string gOutFolder = "/Volumes/data/projects/nautilusnet/data/";
std::string gTextDatasetFile; //= gOutFolder + "textdata.txt";

void storeFramebuffer(const Configuration &config, const char* filename, int clsIdx, int ww, int wh);

enum FILE_TYPE {
	TYPE_PLY,
	TYPE_PCD,
	TYPE_OBJ
};


void ply_cast(void * dest, const char * src, bool be) {
	*(static_cast<unsigned char *>(dest)) = *(reinterpret_cast<const unsigned char *>(src));
}

// int main(int argc, char* args[]) {
// 	Reader *reader = new PLYReader();
// 	Model3D *model;

// 	if( argc < 2) {
// 		//if((model = reader->load("data/00037.ply", 2))== NULL) {
// 		if((model = reader->load("data/09620.ply", 5, false, false))== NULL) {
// 			cout << "Could not load input file!" << endl;
// 			delete reader;
// 			return 1;
// 		}
// 		// reader->save(model, "data/09620_ascii.ply", 32);
// 		reader->save(model, "data/09620_ascii.ply", 30);
// 		cout << "Model saved to data/09620_ascii.ply." << endl;
// 	} else {
// 		int cmd = std::stoi(args[1]);
// 		if((model = reader->load("data/00037.ply", 2, true, true))== NULL) {
// 		// if((model = reader->load("data/simple_model_1.ply", 2, false, false))== NULL) {
// 		// if((model = reader->load("data/simple_binary.ply", 2, false, false))== NULL) {
// 			cout << "Could not load input file!" << endl;
// 			delete reader;
// 			return 1;
// 		}
// 	}

// 	delete model;
// 	delete reader;
// 	return 0;
// }

int main(int argc, char* args[]) {
	Reader *reader;
	Model3D *model;
	Configuration config;
	int needLighting = 1;
	GLfloat pointSize = 3.0f;

	char class_name[16];
	int class_index = 0;
	char out_filename[128];

	GLFWwindow *window;
	Viewer viewer;

	if(argc < 2) {
		cout << "Not enough parameters. \n";
		cout << "USAGE: cap3d <input>.ply OR cap3d <input>.im \n";
		return 1;
	}

	if(argc >= 4) {
		strcpy(class_name, args[2]);
		class_index = std::stoi(args[3]);
	}
	GLint primitive = GL_UNKNOWN_PRIMITIVE;

	FILE_TYPE input_type;
	//Determine the input file type
	if(find(args[1], ".pcd") >=0 ) {
		input_type = TYPE_PCD;
		reader = new PCDReader();
		primitive = GL_POINTS;
	} else if (find(args[1], ".ply") >=0 ) {
		input_type = TYPE_PLY;
		reader = new PLYReader();
		cout << "[DEBUG-CAP3D] Created PLYReader !!!!!!!!!" << endl;
	}

	//Load config file - hardcode filename here!
	if( read_configuration("nautilus.cfg", config) != OK ) {
		cout << "Could not read configuration file!" << endl;
		delete reader;
		return 1;
	}

	gTextDatasetFile = config.output_folder + "textdata.txt";

	cout << "[DEBUG-CAP3D] window_width: " << config.window_width << endl;
	cout << "[DEBUG-CAP3D] window_height: " << config.window_height << endl;
	cout << "[DEBUG-CAP3D] background: " << config.background[0] << ", " << config.background[1] << ", " << config.background[2] << endl;
	cout << "[DEBUG-CAP3D] scale: " << config.scale_factor << endl;
	cout << "[DEBUG-CAP3D] number of camera: " << config.camera_positions.size() << endl;
	for(int i=0; i<config.camera_positions.size(); i++) {
		cout << config.camera_positions[i][0] << ", " << config.camera_positions[i][1] << ", " << config.camera_positions[i][2] << endl;
	}
	cout << "[DEBUG-CAP3D] lightpos_1: " << config.lightpos1[0] << ", " << config.lightpos1[1] << ", " << config.lightpos1[2] << endl;
	cout << "[DEBUG-CAP3D] lightpos_2: " << config.lightpos2[0] << ", " << config.lightpos2[1] << ", " << config.lightpos2[2] << endl;
	cout << "[DEBUG-CAP3D] Output folder: " << config.output_folder << endl;
	// return 0;

	if((model = reader->load(args[1], config.scale_factor, true, true))== NULL) {
		cout << "Could not load input file!" << endl;
		delete reader;
		return 1;
	}

	// cout << "[DEBUG-CAP3D] GOT HERE!!" << endl;

	//We don't need the reader anymore so delete here!
	delete reader;

	BBox3d bbox;
	model->getBBox(bbox);
	cout << "[DEBUG-CAP3D] Bounding Box (left, top, right, bottom) = (" << bbox.minx << ", ";
	cout << bbox.miny << ", " << bbox.maxx << ", " << bbox.maxy << ")" << endl;

	glm::vec3 object_center = glm::vec3((bbox.minx + bbox.maxx)/2.0f,
											(bbox.miny + bbox.maxy)/2.0f,
										(bbox.minz + bbox.maxz)/2.0f);
	// cout << "Center of object: " << object_center << endl;

	//TODO: Do we need to do this???
	model->translate(-object_center.x, -object_center.y, -object_center.z);
	object_center = glm::vec3(0.0f, 0.0f, 0.0f);

	// VBO vbo(model, primitive, GL_STATIC_DRAW);
	// VBO vbo(model, GL_TRIANGLES, GL_STATIC_DRAW);

	if(config.camera_positions.size() < 1) {
		cout << "At least one camera position defined." << endl;
		delete model;
		return -1;
	}

	// Initialise GLFW
	if( !glfwInit() )	{
		fprintf( stderr, "Failed to initialize GLFW\n" );
		getchar();
		delete model;
		return -1;
	}
	
	glfwWindowHint(GLFW_SAMPLES, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // To make MacOS happy; should not be needed
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	// Open a window and create its OpenGL context
	window = glfwCreateWindow( config.window_width, config.window_height, "Nautilus 3D Object Capturer - 1.0", NULL, NULL);
	if( window == NULL ) {
		fprintf( stderr, "Failed to open GLFW window. If you have an Intel GPU, they are not 3.3 compatible. Try the 2.1 version of the tutorials.\n" );
		getchar();
		glfwTerminate();
		delete model;
		return -1;
	}
	glfwMakeContextCurrent(window);

	// Initialize GLEW
	glewExperimental = true; // Needed for core profile
	if (glewInit() != GLEW_OK) {
		fprintf(stderr, "Failed to initialize GLEW\n");
		getchar();
		glfwTerminate();
		delete model;
		return -1;
	}

	//Control variables
	double xpos, ypos;
	double last_xpos, last_ypos;

	// Ensure we can capture the escape key being pressed below
	glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);
  	// Hide the mouse and enable unlimited mouvement
  	//glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
  	// Set the mouse at the center of the screen
  	glfwPollEvents();
  	glfwSetCursorPos(window, config.window_width/2, config.window_height/2);
	// Dark blue background
	glClearColor(config.background[0], config.background[1], config.background[2], 1.0f);
	// Enable depth test
	glEnable(GL_DEPTH_TEST);
	// Accept fragment if it closer to the camera than the former one
	glDepthFunc(GL_LESS);
	// Cull triangles which normal is not towards the camera
	// glEnable(GL_CULL_FACE);

	//turn on GL_PROGRAM_POINT_SIZE 
	glEnable(GL_PROGRAM_POINT_SIZE);

	// unsigned int num_of_vertex, index_size;
	// float *vertices_buf_data = model->getVertexBuffer(buflen);
	// float *normal_buf_data = model->getNormalBuffer(num_of_vertex);
	// unsigned short *indices = model->getElementIndices(index_size);

	//testing
	// int k;
	// for(int i=0; i<num_of_vertex; i++) {
	// 	k = f.getFloatStride() * i;
	// 	cout << "vertex[" << i << "]: " << vertices_buf_data[k] << ", ";
	// 	cout << vertices_buf_data[k+1] << ", " << vertices_buf_data[k+2];
	//
	// 	cout << " normal:(" << normal_buf_data[k] << ", ";
	// 	cout << normal_buf_data[k+1] << ", " << normal_buf_data[k+2] << ")\n";
	// }
	// cout << "Index size: " << index_size << endl;
	// for(int i=0; i<index_size; i++) {
	// 	cout << indices[i] << ", ";
	// }
	// cout << endl;

	glm::mat4 ModelMatrix, MVP;
	ShaderVarLocation shaderVarLocation;

	GLuint programID = loadShaders( "vertex.shader", "fragment.shader");
	glUseProgram(programID);

	shaderVarLocation.positionLocation = POSITION_LOCATION;
	shaderVarLocation.colorLocation = COLOR_LOCATION;
	shaderVarLocation.normalLocation = NORMAL_LOCATION;

	shaderVarLocation.mvpMatrixId = glGetUniformLocation(programID, "MVP");
	shaderVarLocation.viewMatrixId = glGetUniformLocation(programID, "V");
	shaderVarLocation.modelMatrixId = glGetUniformLocation(programID, "M");
	shaderVarLocation.useNormalLocation = glGetUniformLocation(programID, "useNormal");
	shaderVarLocation.useLightingLocation = glGetUniformLocation(programID, "useLighting");
	//TODO: pointSizeLocation is a float??????
	shaderVarLocation.pointSizeLocation = glGetUniformLocation(programID, "pointSize");
	// Get handles for our "lightPos" uniforms, we're going to use two lights
	shaderVarLocation.lightPos1ID = glGetUniformLocation(programID, "lightPos1_worldspace");
	shaderVarLocation.lightPos2ID = glGetUniformLocation(programID, "lightPos2_worldspace");
	shaderVarLocation.lightColor1ID = glGetUniformLocation(programID, "lightColor1");
	shaderVarLocation.lightColor2ID = glGetUniformLocation(programID, "lightColor2");

	//glm::vec3 lightPos = glm::vec3(7,7,7);
	glm::vec3 lightPos1 = glm::vec3(config.lightpos1[0], config.lightpos1[1], config.lightpos1[2]);
	glm::vec3 lightColor1 = glm::vec3(0.5f, 0.5f, 0.1f);
	glm::vec3 lightPos2 = glm::vec3(config.lightpos2[0], config.lightpos2[1], config.lightpos2[2]);
	glm::vec3 lightColor2 = glm::vec3(0.0f, 0.6f, 0.9f);

	double lastTime = glfwGetTime();
	double currentTime;
	double delta;
	double lastFrameTime = lastTime;
	unsigned int frameCount = 0;

	glm::vec3 orientation1;
	
	glm::mat4 inverRotMatrix;
	glm::mat4 modelMatrix;
	glm::mat4 translationMatrix;
	glm::mat4 invertTranslationMatrix;
	glm::mat4 scalingMatrix, rotationMatrix;
	// glm::vec3 position1(-1.5f, 0.0f, 0.0f);
	int button_state;
	bool should_store_frame_buffer = false;

	bool is_control_down = false;
	double i_o_time, i_o_last_time = lastTime;
	double p_key_press_time, l_key_press_time;
	double p_last_key_press_time = lastTime;
	double l_last_key_press_time = lastTime;

	//glm::vec3 cam_pos = glm::vec3(0.0f, 7.0f, 0.0f);
	glm::vec3 cam_pos = config.camera_positions[0];
	int cam_pos_count = config.camera_positions.size();
	int cam_pos_i = 0;

	glfwGetCursorPos(window, &xpos, &ypos);
	last_xpos = xpos;
	last_ypos = ypos;

	viewer.setFW3Window(window);

	//Set locations to viewer
	viewer.setLocations(shaderVarLocation);
	viewer.setupCamera(config.window_width, config.window_height, cam_pos, object_center);
	viewer.setupCoordinator();
	viewer.addModel(model);

	do {
		currentTime = glfwGetTime();
		delta = currentTime - lastFrameTime;
		lastFrameTime = currentTime;
		frameCount++;
		if(currentTime - lastTime >= 1.0) {
			frameCount = 0;
			lastTime += 1.0;
		}
		// Clear the screen
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		// Use our shader
		glUseProgram(programID);

		// P key for printing to file
		if (glfwGetKey( window, GLFW_KEY_P ) == GLFW_PRESS) {
			p_key_press_time = glfwGetTime();
			if(p_key_press_time - p_last_key_press_time >= 0.5) {
				// position -= right * deltaTime * speed;
				should_store_frame_buffer = true;
				cam_pos_i = 0;
				cout << "Key P pressed!" << endl;
				p_last_key_press_time = p_key_press_time;
			}
		}

		if(should_store_frame_buffer) {
			cam_pos = config.camera_positions[cam_pos_i];
			getFrameName(cam_pos_i, class_name, out_filename);
		}

		//Need lighting??
		if(glfwGetKey( window, GLFW_KEY_L ) == GLFW_PRESS) {
			l_key_press_time = glfwGetTime();
			if(l_key_press_time - l_last_key_press_time >= 0.5) {
				needLighting = (needLighting == 0)?1:0;
				l_last_key_press_time = l_key_press_time;
			}
		}

		/////////////////////Start Drawing//////////////////////////////	

		if(glfwGetKey( window, GLFW_MOD_SHIFT ) == GLFW_PRESS) {
			is_control_down = true;
		}

		if(glfwGetKey( window, GLFW_MOD_SHIFT ) == GLFW_RELEASE) {
			is_control_down = false;
		}

		if(glfwGetKey( window, GLFW_KEY_I ) == GLFW_PRESS) {
			i_o_time = glfwGetTime();
			if(i_o_time - i_o_last_time > 0.4) {
				pointSize += 0.5f;
				i_o_last_time = i_o_time;
			}
		}

		if(glfwGetKey( window, GLFW_KEY_O ) == GLFW_PRESS) {
			i_o_time = glfwGetTime();
			if(i_o_time - i_o_last_time > 0.4) {
				pointSize -= 0.5f;
				i_o_last_time = i_o_time;
			}
		}	

		glUniform3f(shaderVarLocation.lightPos1ID, lightPos1.x, lightPos1.y, lightPos1.z);
		glUniform3f(shaderVarLocation.lightPos2ID, lightPos2.x, lightPos2.y, lightPos2.z);
		glUniform3f(shaderVarLocation.lightColor1ID, lightColor1.x, lightColor1.y, lightColor1.z);
		glUniform3f(shaderVarLocation.lightColor2ID, lightColor2.x, lightColor2.y, lightColor2.z);

		// glUniform1i(shaderVarLocation.useNormalLocation, 10);
		// glUniform1i(shaderVarLocation.useLightingLocation, 10);
		glUniform1f(shaderVarLocation.pointSizeLocation, pointSize);

		glfwGetCursorPos(window, &xpos, &ypos);
		button_state = glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT);
		if(button_state == GLFW_PRESS) {
			// Compute new orientation
			// verticalAngle = -1 * mouseSpeed * float(last_xpos - xpos)/2.0f;
			// horizontalAngle = -1 * mouseSpeed * float(last_ypos - ypos)/2.0f;

			verticalAngle = -1 * mouseSpeed * float(last_xpos - xpos)/2.0f;
			horizontalAngle = -1 * mouseSpeed * float(last_ypos - ypos)/2.0f;

			glm::vec4 xrotv = glm::inverse(rotationMatrix) * glm::vec4(1.0f, 0.0f, 0.0f, 1.0f);
			rotationMatrix = glm::rotate(rotationMatrix, horizontalAngle, glm::vec3(xrotv.x, xrotv.y, xrotv.z));
			glm::vec4 yrotv = glm::inverse(rotationMatrix) * glm::vec4(0.0f, 1.0f, 0.0f, 1.0f);
			rotationMatrix = glm::rotate(rotationMatrix, verticalAngle, glm::vec3(yrotv.x, yrotv.y, yrotv.z));
		}

		last_xpos = xpos;
		last_ypos = ypos;

		// Move forward
		if (glfwGetKey( window, GLFW_KEY_UP ) == GLFW_PRESS) {
			horizontalAngle = -1 * speed * 0.005;
			glm::vec4 xrotv = glm::inverse(rotationMatrix) * glm::vec4(1.0f, 0.0f, 0.0f, 1.0f);
			rotationMatrix = glm::rotate(rotationMatrix, horizontalAngle, glm::vec3(xrotv.x, xrotv.y, xrotv.z));
		}
		// Move backward
		if (glfwGetKey( window, GLFW_KEY_DOWN ) == GLFW_PRESS) {
			horizontalAngle = -1 * speed * -0.005;
			glm::vec4 xrotv = glm::inverse(rotationMatrix) * glm::vec4(1.0f, 0.0f, 0.0f, 1.0f);
			rotationMatrix = glm::rotate(rotationMatrix, horizontalAngle, glm::vec3(xrotv.x, xrotv.y, xrotv.z));
		}
		// Strafe right
		if (glfwGetKey( window, GLFW_KEY_RIGHT ) == GLFW_PRESS) {
			// position += right * deltaTime * speed;
			verticalAngle = -1 * speed * 0.005f;
			glm::vec4 yrotv = glm::inverse(rotationMatrix) * glm::vec4(0.0f, 1.0f, 0.0f, 1.0f);
			rotationMatrix = glm::rotate(rotationMatrix, verticalAngle, glm::vec3(yrotv.x, yrotv.y, yrotv.z));
		}
		// Strafe left
		if (glfwGetKey( window, GLFW_KEY_LEFT ) == GLFW_PRESS) {
			// position -= right * deltaTime * speed;
			verticalAngle = -1 * speed * -0.005f;
			glm::vec4 yrotv = glm::inverse(rotationMatrix) * glm::vec4(0.0f, 1.0f, 0.0f, 1.0f);
			rotationMatrix = glm::rotate(rotationMatrix, verticalAngle, glm::vec3(yrotv.x, yrotv.y, yrotv.z));
		}

		// translationMatrix = glm::translate(glm::mat4(), position1);
		//translationMatrix = glm::translate(glm::mat4(), object_center);
		//invertTranslationMatrix = glm::translate(glm::mat4(), - object_center);
		// scalingMatrix = glm::scale(glm::mat4(), glm::vec3(.5f, .5f, .5f));
		// modelMatrix = translationMatrix * rotationMatrix * scalingMatrix;
		//modelMatrix = translationMatrix * rotationMatrix * invertTranslationMatrix * scalingMatrix;
		//MVP = projectionMatrix * viewMatrix * modelMatrix;
		
		viewer.drawScene();

		//TODO: Call glReadPixels to capture framebuffer data here
		if(should_store_frame_buffer) {
			storeFramebuffer(config, out_filename, class_index, config.window_width, config.window_height);
			cam_pos_i++;
			if(cam_pos_i >= cam_pos_count)
				should_store_frame_buffer = false;
		}

		// Swap buffers
		glfwSwapBuffers(window);
		glfwPollEvents();

 // Check if the ESC key was pressed or the window was closed
	} while( glfwGetKey(window, GLFW_KEY_ESCAPE ) != GLFW_PRESS &&
			 glfwWindowShouldClose(window) == 0 );

	glDeleteProgram(programID);
	
	// Close OpenGL window and terminate GLFW
	glfwTerminate();

	// delete[] vertices_buf_data;
	// delete[] normal_buf_data;
	// delete[] indices;
	delete model;
	return 0;
}

/**
	Params:
		int ww [IN] windows width
		int wh [IN] windows height
*/
void storeFramebuffer(const Configuration &config, const char* filename, int clsIdx, int ww, int wh) {
	cv::Mat img(wh, ww, CV_8UC3, cv::Scalar(0, 0, 0));
	cv::Mat flipped(wh, ww, CV_8UC3, cv::Scalar(0, 0, 0));

	// glBindFramebuffer(GL_READ_FRAMEBUFFER, 0);
	glPixelStorei(GL_PACK_ALIGNMENT, (img.step & 3)?1:4);
	glPixelStorei(GL_PACK_ROW_LENGTH, img.step/img.elemSize());
	glReadPixels(0, 0, ww, wh, GL_BGR, GL_UNSIGNED_BYTE, img.data);
	cv::Rect bbox;
	cv::Scalar bbcolor(0, 0, 255);
	// ofstream txtf("textdata.txt", std::ios_base::app);
	ofstream txtf(gTextDatasetFile.c_str(), std::ios_base::app);

	if(txtf.fail()) {
		cout << "Could not open text output file!\n";
		return;
	}
	//flip the image around x-axis
	cv::flip(img, flipped, 0);
	detectBoundingBox(flipped, img.at<cv::Vec3b>(0,0), bbox);
	cout << "Bouding Box: " << bbox.x << ", " << bbox.y << ", " << bbox.width << ", " << bbox.height << endl;
	//txtf << gOutFolder << filename << " " << bbox.x << " " << bbox.y << " " << (bbox.x + bbox.width) << " " << (bbox.y + bbox.height) << " " << clsIdx << endl;
	txtf << config.output_folder << filename << " " << bbox.x << " " << bbox.y << " " << (bbox.x + bbox.width) << " " << (bbox.y + bbox.height) << " " << clsIdx << endl;
	txtf.close();
	//cv::rectangle(flipped, bbox, bbcolor, 1, cv::LINE_8, 0 );
	std::string outImageName = config.output_folder + filename;
	imwrite(outImageName, flipped);
	// imwrite(filename, flipped);
}
