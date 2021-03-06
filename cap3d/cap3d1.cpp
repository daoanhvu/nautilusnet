#include <iostream>
#include <fstream>
#include <exception>
#include <string>
#include <cstring>
#include <sstream>
#include <vector>
#include <camera.h>

#include "reader.h"
#include "model3d.h"
#include <glrenderer.h>
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

#include "lexer_parser.h"
#include "shader.h"
#include "image_processor.h"
#include "utils.h"

#define PI 3.14159f
#define RAD(x) ( x * 3.14159f / 180.0f )
#define BYTES_PER_PROP 4

#define OK 0

#define OS_UNKNOWN 0
#define OS_X 1
#define LINUX 2
#define WINDOWS 3


using namespace std;
using namespace fp;
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

/**
	Build a file name with format: <classname>_<poseNumber>.jpg
*/
void getFrameName(int idx, const char *cls, char *name) {
	int i, len, cls_len = strlen(cls);
	memcpy(name, cls, cls_len);
	name[cls_len++] = '_';
	len = itostr(idx, name, cls_len);
	name[len] = '.';
	name[len+1] = 'j';
	name[len+2] = 'p';
	name[len+3] = 'g';
	name[len+4] = '\0';
}

void computeMatrices(GLFWwindow* window, glm::vec3 lookat);
void storeFramebuffer(const Configuration &config, const char* filename, int clsIdx, int ww, int wh);

enum FILE_TYPE {
	TYPE_PLY,
	TYPE_PCD,
	TYPE_OBJ
};

int main(int argc, char* args[]) {
	Reader f;
	Model3D *model;
	unsigned int buflen;
	Configuration config;

	char class_name[16];
	int class_index = 0;
	char out_filename[128];

	if(argc < 2) {
		cout << "Not enough parameters. \n";
		cout << "USAGE: cap3d <input>.ply OR cap3d <input>.im \n";
		return 1;
	}

	if(argc >= 4) {
		strcpy(class_name, args[2]);
		class_index = std::stoi(args[3]);
	}

	FILE_TYPE input_type;
	//Determine the input file type
	if(find(args[1], ".pcd")) {
		input_type = TYPE_PCD;
	} else if (find(args[1], ".ply")) {
		input_type = TYPE_PLY;
	}

	//Load config file - hardcode filename here!
	if( read_configuration("nautilus.cfg", config) != OK ) {
		cout << "Could not read configuration file!" << endl;
		return 1;
	}

	gTextDatasetFile = config.output_folder + "textdata.txt";

	cout << "window_width: " << config.window_width << endl;
	cout << "window_height: " << config.window_height << endl;
	cout << "background: " << config.background[0] << ", " << config.background[1] << ", " << config.background[2] << endl;
	cout << "scale: " << config.scale_factor << endl;
	cout << "number of camera: " << config.camera_positions.size() << endl;
	for(int i=0; i<config.camera_positions.size(); i++) {
		cout << config.camera_positions[i][0] << ", " << config.camera_positions[i][1] << ", " << config.camera_positions[i][2] << endl;
	}
	cout << "lightpos_1: " << config.lightpos1[0] << ", " << config.lightpos1[1] << ", " << config.lightpos1[2] << endl;
	cout << "lightpos_2: " << config.lightpos2[0] << ", " << config.lightpos2[1] << ", " << config.lightpos2[2] << endl;
	cout << "Output folder: " << config.output_folder << endl;
	// return 0;

	if((model = f.load(args[1]) )== NULL) {
		cout << "Could not load input file!" << endl;
		delete model;
		return 1;
	}

	model->scaleToFit(2.0f);

	// f.print(cout);
	model->add_normal_vectors();

	BBox3d bbox;
	model->getBBox(bbox);
	cout << "Bounding Box (left, top, right, bottom) = (" << bbox.minx << ", ";
	cout << bbox.miny << ", " << bbox.maxx << ", " << bbox.maxy << ")" << endl;

	glm::vec3 object_center = glm::vec3((bbox.minx + bbox.maxx)/2.0f,
											(bbox.miny + bbox.maxy)/2.0f,
										(bbox.minz + bbox.maxz)/2.0f);
	// cout << "Center of object: " << object_center << endl;

	//TODO: Do we need to do this???
	model->translate(-object_center.x, -object_center.y, -object_center.z);
	object_center = glm::vec3(0.0f, 0.0f, 0.0f);

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
	GLFWwindow *window;

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
  glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
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
	glEnable(GL_CULL_FACE);

	unsigned int num_of_vertex, index_size;
	float *vertices_buf_data = model->getVertexBuffer(buflen);
	float *normal_buf_data = model->getNormalBuffer(num_of_vertex);
	unsigned short *indices = model->getElementIndices(index_size);

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

	//Setup data for rendering
	GLuint vertexArrayId;
	glGenVertexArrays(1, &vertexArrayId);
	glBindVertexArray(vertexArrayId);

	//Coordinator
	GLuint axes_vertex_buffer;
	glGenBuffers(1, &axes_vertex_buffer);
	float axesVertices[] = {
			0, 0, 0, 0, 1, 0, 1, 0, 0,	1, 0, 0, 0, 1, 0, 1, 0, 0, 
			0, 0, 0, 1, 0, 0, 0, 1, 0,	0, 1, 0, 1, 0, 0, 0, 1, 0,
			0, 0, 0, 0, 1, 0, 0, 0, 1,	0, 0, 1, 0, 1, 0, 0, 0, 1
			};
	glBindBuffer(GL_ARRAY_BUFFER, axes_vertex_buffer);
	glBufferData(GL_ARRAY_BUFFER, 54 * sizeof(float), axesVertices, GL_STATIC_DRAW);


	// Load it into a VBO
	GLuint vertex_buffer;
	glGenBuffers(1, &vertex_buffer);
	glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer);
	glBufferData(GL_ARRAY_BUFFER, num_of_vertex * sizeof(glm::vec3), vertices_buf_data, GL_STATIC_DRAW);

	GLuint normal_buffer;
	glGenBuffers(1, &normal_buffer);
	glBindBuffer(GL_ARRAY_BUFFER, normal_buffer);
	glBufferData(GL_ARRAY_BUFFER, num_of_vertex * sizeof(glm::vec3), normal_buf_data, GL_STATIC_DRAW);

	GLuint element_buffer;
	glGenBuffers(1, &element_buffer);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, element_buffer);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, index_size * sizeof(unsigned short), indices , GL_STATIC_DRAW);

	GLuint programID = loadShaders( "vertex1.shader", "fragment1.shader");
	GLuint mvpMatrixId = glGetUniformLocation(programID, "MVP");
	GLuint viewMatrixId = glGetUniformLocation(programID, "V");
	GLuint modelMatrixId = glGetUniformLocation(programID, "M");

	// Get handles for our "lightPos" uniforms, we're going to use two lights
	glUseProgram(programID);
	GLuint lightPos1ID = glGetUniformLocation(programID, "lightPos1_worldspace");
	GLuint lightPos2ID = glGetUniformLocation(programID, "lightPos2_worldspace");
	GLuint lightColor1ID = glGetUniformLocation(programID, "lightColor1");
	GLuint lightColor2ID = glGetUniformLocation(programID, "lightColor2");

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
	glm::mat4 rotationMatrix = glm::mat4(1.0f);
	glm::mat4 inverRotMatrix;
	glm::mat4 modelMatrix;
	glm::mat4 translationMatrix;
	glm::mat4 invertTranslationMatrix;
	glm::mat4 scalingMatrix;
	// glm::vec3 position1(-1.5f, 0.0f, 0.0f);
	int button_state;
	bool should_store_frame_buffer = false;

	double key_press_time;
	double last_key_press_time = lastTime;

	//glm::vec3 cam_pos = glm::vec3(0.0f, 7.0f, 0.0f);
	glm::vec3 cam_pos = config.camera_positions[0];
	int cam_pos_count = config.camera_positions.size();
	int cam_pos_i = 0;

	glfwGetCursorPos(window, &xpos, &ypos);
	last_xpos = xpos;
	last_ypos = ypos;

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
			key_press_time = glfwGetTime();
			if(key_press_time - last_key_press_time >= 0.5) {
				// position -= right * deltaTime * speed;
				should_store_frame_buffer = true;
				cam_pos_i = 0;
				cout << "Key P pressed!" << endl;
				last_key_press_time = key_press_time;
			}
		}

		if(should_store_frame_buffer) {
			cam_pos = config.camera_positions[cam_pos_i];
			getFrameName(cam_pos_i, class_name, out_filename);
		}

		//computeMatrices(window, object_center);
		projectionMatrix = glm::perspective(glm::radians(45.0f), 4.0f / 3.0f, 0.1f, 100.0f);
		viewMatrix = glm::lookAt(
			cam_pos,
			object_center,
			glm::vec3(0, 1, 0)
		);
		/////////////////////Start Drawing//////////////////////////////

		//Draw Coordinator
		// glEnableVertexAttribArray(0);
		// glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer);
		// glVertexAttribPointer(0, //Attribute index
		// 	3,  //Number of component per vertex
		// 	GL_FLOAT,
		// 	GL_FALSE,
		// 	0,
		// 	(void*)0);

		// GLES20.glDrawArrays(GLES20.GL_LINES, 0, 6);

		/////////////////////Start Drawing//////////////////////////////		

		glUniform3f(lightPos1ID, lightPos1.x, lightPos1.y, lightPos1.z);
		glUniform3f(lightPos2ID, lightPos2.x, lightPos2.y, lightPos2.z);
		glUniform3f(lightColor1ID, lightColor1.x, lightColor1.y, lightColor1.z);
		glUniform3f(lightColor2ID, lightColor2.x, lightColor2.y, lightColor2.z);

		glEnableVertexAttribArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer);
		glVertexAttribPointer(0, //Attribute index
			3,  //Number of component per vertex
			GL_FLOAT,
			GL_FALSE,
			0,
			(void*)0);

		//Normal data
		glEnableVertexAttribArray(1);
		glBindBuffer(GL_ARRAY_BUFFER, normal_buffer);
		glVertexAttribPointer(1, //Attribute index
			3,  //Number of component per vertex
			GL_FLOAT,
			GL_FALSE,
			0,
			(void*)0);

		// Index buffer
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, element_buffer);

		glfwGetCursorPos(window, &xpos, &ypos);
		button_state = glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT);
		if(button_state == GLFW_PRESS) {
			// Compute new orientation
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
		translationMatrix = glm::translate(glm::mat4(), object_center);
		invertTranslationMatrix = glm::translate(glm::mat4(), - object_center);
		// scalingMatrix = glm::scale(glm::mat4(), glm::vec3(.5f, .5f, .5f));
		// modelMatrix = translationMatrix * rotationMatrix * scalingMatrix;
		modelMatrix = translationMatrix * rotationMatrix * invertTranslationMatrix * scalingMatrix;
		MVP = projectionMatrix * viewMatrix * modelMatrix;

		// Send our transformation to the currently bound shader,
		// in the "MVP" uniform
		glUniformMatrix4fv(mvpMatrixId, 1, GL_FALSE, &MVP[0][0]);
		glUniformMatrix4fv(modelMatrixId, 1, GL_FALSE, &modelMatrix[0][0]);
		glUniformMatrix4fv(viewMatrixId, 1, GL_FALSE, &viewMatrix[0][0]);

		//Now, we draw the model
		// user GL_POINTS to draw pixel per pixel
		//glDrawArrays(GL_TRIANGLES, 0, num_of_vertex);
		glDrawElements(
				GL_TRIANGLES,      // mode
				index_size,    // count
				GL_UNSIGNED_SHORT,   // type
				(void*)0           // element array buffer offset
			);

		//TODO: Call glReadPixels to capture framebuffer data here
		if(should_store_frame_buffer) {
			storeFramebuffer(config, out_filename, class_index, config.window_width, config.window_height);
			cam_pos_i++;
			if(cam_pos_i >= cam_pos_count)
				should_store_frame_buffer = false;
		}

		glDisableVertexAttribArray(0);
		glDisableVertexAttribArray(1);

		// Swap buffers
		glfwSwapBuffers(window);
		glfwPollEvents();

 // Check if the ESC key was pressed or the window was closed
	} while( glfwGetKey(window, GLFW_KEY_ESCAPE ) != GLFW_PRESS &&
			 glfwWindowShouldClose(window) == 0 );

	glDeleteBuffers(1, &vertex_buffer);
	glDeleteBuffers(1, &normal_buffer);
	glDeleteBuffers(1, &element_buffer);
	glDeleteProgram(programID);
	glDeleteVertexArrays(1, &vertexArrayId);

	// Close OpenGL window and terminate GLFW
	glfwTerminate();

	delete[] vertices_buf_data;
	delete[] normal_buf_data;
	delete[] indices;
	delete model;
	return 0;
}

/*
	(mx, my) mouse position
*/
void computeMatrices(GLFWwindow* window, glm::vec3 lookat) {
	// glfwGetTime is called only once, the first time this function is called
	static double lastTime = glfwGetTime();

	// Compute time difference between current and last frame
	double currentTime = glfwGetTime();
	float deltaTime = float(currentTime - lastTime);

	// Get mouse position
	double xpos, ypos;
	glfwGetCursorPos(window, &xpos, &ypos);

	// Reset mouse position for next frame
	glfwSetCursorPos(window, 1024/2, 768/2);

	// Compute new orientation
	horizontalAngle += mouseSpeed * float(1024/2 - xpos );
	verticalAngle   += mouseSpeed * float( 768/2 - ypos );

	// Direction : Spherical coordinates to Cartesian coordinates conversion
	glm::vec3 direction(
		cos(verticalAngle) * sin(horizontalAngle),
		sin(verticalAngle),
		cos(verticalAngle) * cos(horizontalAngle)
	);

	// Right vector
	float PIDIV2 = PI / 2.0f;
	glm::vec3 right = glm::vec3(
		sin(horizontalAngle - PIDIV2),
		0,
		cos(horizontalAngle - PIDIV2)
	);

	// Up vector
	glm::vec3 up = glm::cross( right, direction );

	// Move forward
	if (glfwGetKey( window, GLFW_KEY_UP ) == GLFW_PRESS){
		position += direction * deltaTime * speed;
	}
	// Move backward
	if (glfwGetKey( window, GLFW_KEY_DOWN ) == GLFW_PRESS){
		position -= direction * deltaTime * speed;
	}
	// Strafe right
	if (glfwGetKey( window, GLFW_KEY_RIGHT ) == GLFW_PRESS){
		position += right * deltaTime * speed;
	}
	// Strafe left
	if (glfwGetKey( window, GLFW_KEY_LEFT ) == GLFW_PRESS){
		position -= right * deltaTime * speed;
	}

	float FoV = initialFoV;// - 5 * glfwGetMouseWheel(); // Now GLFW 3 requires setting up a callback for this. It's a bit too complicated for this beginner's tutorial, so it's disabled instead.

	// Projection matrix : 45� Field of View, 4:3 ratio, display range : 0.1 unit <-> 100 units
	projectionMatrix = glm::perspective(glm::radians(FoV), 4.0f / 3.0f, 0.1f, 100.0f);
	// Camera matrix
	viewMatrix       = glm::lookAt(
								position,           // Camera is here
								lookat, // and looks here : at the same position, plus "direction"
								up                  // Head is up (set to 0,-1,0 to look upside-down)
							 );

	// For the next frame, the "last time" will be "now"
	lastTime = currentTime;
}

/**
	Params:
		int ww [IN] windows width
		int wh [IN] windows height
*/
void storeFramebuffer(const Configuration &config, const char* filename, int clsIdx, int ww, int wh) {
	int i, j;
	int k;
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
