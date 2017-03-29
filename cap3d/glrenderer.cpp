#include <glrenderer.h>

//my own lib
#include <camera.h>
/*
	This method use glReadPixels to capture data from a frame
	http://www.opengl-tutorial.org/beginners-tutorials/tutorial-2-the-first-triangle/

*/

GLuint loadShaders(const char* vertex_shader_file, const char* fragment_shader_file) {
	GLuint vertexShaderId = glCreateShader(GL_VERTEX_SHADER);
	GLuint fragmentShaderId = glCreateShader(GL_FRAGMENT_SHADER);
	return 0;
}

void setupGl() {

}

void storeScene(const fp::Camera* cam) {

}