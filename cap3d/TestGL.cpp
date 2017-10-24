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
#include "shader.h"
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

int main(int argc, char* args[]) {

	//Control variables
	double xpos, ypos;
	double last_xpos, last_ypos;

	glm::mat4 ModelMatrix, MVP;
	glm::mat4 viewMatrix;
	glm::mat4 projectionMatrix;
	
	//For testing 
	float vertex_data[] = {	
		-0.555556f, 1.f, 0.222222f, -0.82966f, 0.345089f, 0.438837f, 0.3f, 0.5f, 0.6f,
		-0.555556f, -1.f, -1.f, -0.917408f, -0.368747f, 0.149625f, 0.3f, 0.5f, 0.6f,
		0.555556f, 0.555556f, 1.f, 0.682326f, 0.715434f, 0.150282f, 0.3f, 0.5f, 0.6f,
		-0.555556f, 1.f, -0.666667f, 0.180382f, 0.87033f, -0.458244f, 0.3f, 0.5f, 0.6f
	};
	unsigned int indices[] = {1, 2, 0, 1, 0, 3, 2, 3, 0, 1, 3, 2};
	int float_stride = 9;
	int stride_in_byte = float_stride * sizeof(float);
	unsigned int num_of_vertex = 4, index_size = 12;

	// Initialise GLFW
	if( !glfwInit() )	{
		fprintf( stderr, "Failed to initialize GLFW\n" );
		getchar();
		return -1;
	}
	GLFWwindow *window;

	glfwWindowHint(GLFW_SAMPLES, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // To make MacOS happy; should not be needed
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	// Open a window and create its OpenGL context
	window = glfwCreateWindow( 800, 600, "Nautilus 3D Object Capturer - 1.0", NULL, NULL);
	if( window == NULL ) {
		fprintf( stderr, "Failed to open GLFW window. If you have an Intel GPU, they are not 3.3 compatible. Try the 2.1 version of the tutorials.\n" );
		getchar();
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);

	// Initialize GLEW
	glewExperimental = true; // Needed for core profile
	if (glewInit() != GLEW_OK) {
		fprintf(stderr, "Failed to initialize GLEW\n");
		getchar();
		glfwTerminate();
		return -1;
	}

	// Ensure we can capture the escape key being pressed below
	glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);
    // Hide the mouse and enable unlimited mouvement
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    // Set the mouse at the center of the screen
    glfwPollEvents();
    glfwSetCursorPos(window, 400, 300);
	// Dark blue background
	glClearColor(0.4f, 0.4f, 0.4f, 1.0f);
	// Enable depth test
	glEnable(GL_DEPTH_TEST);
	// Accept fragment if it closer to the camera than the former one
	glDepthFunc(GL_LESS);
	// Cull triangles which normal is not towards the camera
	glEnable(GL_CULL_FACE);

	//Setup data for rendering
	GLuint vertexArrayId;
	glGenVertexArrays(1, &vertexArrayId);
	glBindVertexArray(vertexArrayId);

	// Load it into a VBO
	GLuint vertex_buffer;
	glGenBuffers(1, &vertex_buffer);
	glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer);
	glBufferData(GL_ARRAY_BUFFER, num_of_vertex * stride_in_byte, vertex_data, GL_STATIC_DRAW);

	GLuint element_buffer;
	glGenBuffers(1, &element_buffer);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, element_buffer);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, index_size * sizeof(unsigned int), indices , GL_STATIC_DRAW);

	GLuint programID = loadShaders( "vertex.shader", "fragment.shader");
	glUseProgram(programID);


	GLuint mvpMatrixId = glGetUniformLocation(programID, "MVP");
	GLuint viewMatrixId = glGetUniformLocation(programID, "V");
	GLuint modelMatrixId = glGetUniformLocation(programID, "M");

	GLuint useNormalLocation = glGetUniformLocation(programID, "useNormal");
	GLuint useLightingLocation = glGetUniformLocation(programID, "useLighting");
	//TODO: pointSizeLocation is a float??????
	GLuint pointSizeLocation = glGetUniformLocation(programID, "pointSize");
	// Get handles for our "lightPos" uniforms, we're going to use two lights
	GLuint lightPos1ID = glGetUniformLocation(programID, "lightPos1_worldspace");
	GLuint lightPos2ID = glGetUniformLocation(programID, "lightPos2_worldspace");
	GLuint lightColor1ID = glGetUniformLocation(programID, "lightColor1");
	GLuint lightColor2ID = glGetUniformLocation(programID, "lightColor2");

	//glm::vec3 lightPos = glm::vec3(7,7,7);
	glm::vec3 lightPos1 = glm::vec3(1, 2.5, 2);
	glm::vec3 lightColor1 = glm::vec3(0.5f, 0.5f, 0.1f);
	glm::vec3 lightPos2 = glm::vec3(2, 4, 8);
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
	glm::vec3 cam_pos = glm::vec3(2, 1, 2.4);

	glfwGetCursorPos(window, &xpos, &ypos);
	last_xpos = xpos;
	last_ypos = ypos;

	glm::vec3 object_center = glm::vec3(0.0f, 0.0f, 0.0f);
	projectionMatrix = glm::perspective(glm::radians(45.0f), 4.0f / 3.0f, 0.1f, 100.0f);
	viewMatrix = glm::lookAt(
		cam_pos,
		object_center,
		glm::vec3(0, 1, 0)
	);

	do {
		
		// Clear the screen
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		// Use our shader
		glUseProgram(programID);

		glBindVertexArray(vertexArrayId);

		glUniform3f(lightPos1ID, lightPos1.x, lightPos1.y, lightPos1.z);
		glUniform3f(lightPos2ID, lightPos2.x, lightPos2.y, lightPos2.z);
		glUniform3f(lightColor1ID, lightColor1.x, lightColor1.y, lightColor1.z);
		glUniform3f(lightColor2ID, lightColor2.x, lightColor2.y, lightColor2.z);

		glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer);

		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, //Attribute index
			3,  //Number of component per vertex
			GL_FLOAT,
			GL_FALSE,
			stride_in_byte,
			reinterpret_cast<void*>(0));

		//Normal data
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, //Attribute index
			3,  //Number of component per vertex
			GL_FLOAT,
			GL_FALSE,
			stride_in_byte,
			reinterpret_cast<void*>(12));

		//Color data
		glEnableVertexAttribArray(2);
		glVertexAttribPointer(2, //Attribute index
			3,  //Number of component per vertex
			GL_FLOAT,
			GL_FALSE,
			stride_in_byte,
			reinterpret_cast<void*>(24));

		// translationMatrix = glm::translate(glm::mat4(), position1);
		translationMatrix = glm::translate(glm::mat4(), object_center);
		invertTranslationMatrix = glm::translate(glm::mat4(), - object_center);
		// scalingMatrix = glm::scale(glm::mat4(), glm::vec3(.5f, .5f, .5f));
		// modelMatrix = translationMatrix * rotationMatrix * scalingMatrix;
		modelMatrix = translationMatrix * rotationMatrix * invertTranslationMatrix * scalingMatrix;
		MVP = projectionMatrix * viewMatrix * modelMatrix;

		glUniform1i(useNormalLocation, 1);
		glUniform1i(useLightingLocation, 1);

		// Send our transformation to the currently bound shader,
		// in the "MVP" uniform
		glUniformMatrix4fv(mvpMatrixId, 1, GL_FALSE, &MVP[0][0]);
		glUniformMatrix4fv(modelMatrixId, 1, GL_FALSE, &modelMatrix[0][0]);
		glUniformMatrix4fv(viewMatrixId, 1, GL_FALSE, &viewMatrix[0][0]);

		//Now, we draw the model
		// user GL_POINTS to draw pixel per pixel
		//glDrawArrays(GL_TRIANGLES, 0, num_of_vertex);
		// Index buffer
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, element_buffer);
		glDrawElements(
				GL_TRIANGLES,      // mode
				index_size,    // count
				GL_UNSIGNED_INT,   // type
				(void*)0           // element array buffer offset
			);

		glDisableVertexAttribArray(0);
		glDisableVertexAttribArray(1);
		glDisableVertexAttribArray(2);

		// Swap buffers
		glfwSwapBuffers(window);
		glfwPollEvents();

 // Check if the ESC key was pressed or the window was closed
	} while( glfwGetKey(window, GLFW_KEY_ESCAPE ) != GLFW_PRESS &&
			 glfwWindowShouldClose(window) == 0 );

	glDeleteBuffers(1, &vertex_buffer);
	glDeleteBuffers(1, &element_buffer);
	glDeleteProgram(programID);
	glDeleteVertexArrays(1, &vertexArrayId);

	// Close OpenGL window and terminate GLFW
	glfwTerminate();
	return 0;
}

