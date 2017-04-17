#include <iostream>
#include <fstream>
#include <exception>
#include <string>
#include <cstring>
#include <sstream>
#include <camera.h>

#include <plyfile.h>
#include <glrenderer.h>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <opencv2/opencv.hpp>
#include <opencv2/core/core.hpp>

#define RAD(x) ( x * 3.14159f / 180.0f )
#define BYTES_PER_PROP 4

#define OK 0

using namespace std;
using namespace fp;


int main(int argc, char* args[]) {
	PlyFile f;
	f.load2(args[1]);

	f.print(cout);
	f.add_normal_vectors();

	return 0;
}

int main1(int argc, char* args[]) {

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

	glfwWindowHint(GLFW_SAMPLES, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // To make MacOS happy; should not be needed
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	window = glfwCreateWindow(640, 480, "Simple example", NULL, NULL);
	if (!window) {
		//Failed to create window
		glfwTerminate();
		exit(EXIT_FAILURE);
	}

	//Make our window current
	glfwMakeContextCurrent(window);

	//MUST initialize glew
	GLenum err = glewInit();
	if (err != GLEW_OK) {
		cout << "Initialize glew failed!!!!! \n";
		glfwTerminate();
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
	cout << "Before initialization!!!\n";
	renderer.initGL();


	//export 2d images
	//renderer.moveCameraTo(0.0f, 0.0f, 0.0f, 0.5f, 0.5f, 0.5f, &f, "pose1.jpg");
	//renderer.moveCameraTo(0.0f, 0.0f, -1.0f, 0.5f, 0.5f, 0.5f, &f, "pose2.jpg");
	//renderer.moveCameraTo(0.0f, -1.0f, 0.0f, 0.5f, 0.5f, 0.5f, &f, "pose3.jpg");
	//renderer.moveCameraTo(0.0f, -1.0f, -1.0f, 0.5f, 0.5f, 0.5f, &f, "pose4.jpg");
	//renderer.moveCameraTo(1.0f, 0.0f, 0.0f, 0.5f, 0.5f, 0.5f, &f, "pose5.jpg");
	//renderer.moveCameraTo(1.0f, 0.0f, -1.0f, 0.5f, 0.5f, 0.5f, &f, "pose6.jpg");

	GLuint VertexArrayID;
	glGenVertexArrays(1, &VertexArrayID);
	glBindVertexArray(VertexArrayID);

	static const GLfloat g_vertex_buffer_data[] = {
		-1.0f, -1.0f, 0.0f,
		 1.0f, -1.0f, 0.0f,
		 0.0f,  1.0f, 0.0f,
	};

	GLuint vertex_buffer;
	glGenBuffers(1, &vertex_buffer);
	glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer);
	// glBufferData(GL_ARRAY_BUFFER, sizeof(g_vertex_buffer_data), g_vertex_buffer_data, GL_STATIC_DRAW);

	do {
		// Clear the screen. It's not mentioned before Tutorial 02, but it can cause flickering, so it's there nonetheless.
		glClear( GL_COLOR_BUFFER_BIT );

		// Draw nothing, see you in tutorial 2 !
		// glUseProgram(renderer.getProgramId());
		//
		// // 1rst attribute buffer : vertices
		// glEnableVertexAttribArray(0);
		// glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer);
		// glVertexAttribPointer(
		// 	0,
		// 	3,
		// 	GL_FLOAT,
		// 	GL_FLOAT,
		// 	0,				/*stride*/
		// 	(void*)0 /*Array buffer offset*/
		// );
		//
		// // Draw the triangle !
		// glDrawArrays(GL_TRIANGLES, 0, 3); // 3 indices starting at 0 -> 1 triangle
		//
		// glDisableVertexAttribArray(0);

		// Swap buffers
		glfwSwapBuffers(window);
		glfwPollEvents();

 // Check if the ESC key was pressed or the window was closed
	} while( glfwGetKey(window, GLFW_KEY_ESCAPE ) != GLFW_PRESS &&
		   glfwWindowShouldClose(window) == 0 );

	// Cleanup VBO
	glDeleteBuffers(1, &vertex_buffer);
	glDeleteVertexArrays(1, &VertexArrayID);

	// Close OpenGL window and terminate GLFW
	glfwTerminate();
	return 0;
}
