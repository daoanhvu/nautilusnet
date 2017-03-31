#include <glrenderer.h>
#include <iostream>
#include <fstream>

#include <GLFW/glfw3.h>

//#include <glm/gtx/transform.hpp>
#include <glm/gtc/matrix_transform.hpp>
/*
	This method use glReadPixels to capture data from a frame
	http://www.opengl-tutorial.org/beginners-tutorials/tutorial-2-the-first-triangle/

*/

using namespace std;

GLRenderer::GLRenderer() {
	programId = 0;
	vertexShaderId = 0;
	fragmentShaderId = 0;
}

GLRenderer::~GLRenderer() {
	if(this->programId != 0) {
		glDeleteShader(this->vertexShaderId);
		glDeleteShader(this->fragmentShaderId);
		glDeleteProgram(this->programId);
		this->programId = 0;
	}
}

/*
	Params:
		shaderType GL_VERTEX_SHADER or GL_FRAGMENT_SHADER
		source string source code of shader
*/
GLuint GLRenderer::createShader(GLenum shaderType, string source) {
	const char* strSource = source.c_str();
	const GLint src_size = source.size();
	int compile_result = 0;

	GLuint shaderid = glCreateShader(shaderType);
	//GLuint shaderid = glCreateShader(GL_VERTEX_SHADER);

	glShaderSource(shaderid, 1, &strSource, &src_size);
	glCompileShader(shaderid);
	glGetShaderiv(shaderid, GL_COMPILE_STATUS, &compile_result);
	//check for error
	if (compile_result == GL_FALSE) {
		return 0;
	}

	return shaderid;
}
/*
	http://stackoverflow.com/questions/41543891/how-to-get-rendered-image-with-glreadpixels-from-framebuffer-after-calling-gld
*/
int GLRenderer::storeScene(GLuint x, GLuint y, GLuint width, GLuint height) {
	const int byte_per_pixel = 3;
	GLubyte* pixels = new GLubyte[byte_per_pixel * width * height];

	//Set viewport
	glViewport(x, y, width, height);

	//Perspective matrix
	this->pp_matrix = glm::perspective(glm::radians(45.0f), (float)width / (float)height, 0.1f, 100.0f);

	//Setup camera
	view_matrix = glm::lookAt(glm::vec3(4, 3, 3), /* Camera is at (4,3,3), in World Space */
		glm::vec3(0, 0, 0), /* and looks at the origin */
		glm::vec3(0, 1, 0));  /* Head is up (set to 0,-1,0 to look upside-down) */

	//load model
	glm::mat4 model(1.0f);

	glm::mat4 mvp = pp_matrix * view_matrix * model;

	//Start rendering
	GLuint mvp_handler = glGetUniformLocation(this->programId, "mvp");
	glUniformMatrix4fv(mvp_handler, 1, GL_FALSE, &mvp[0][0]);

	/*
		Configure pixel mode
		We will align memory by 1-byte
	*/
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

	//store image in memory
	glReadPixels(0, 0, width, height, GL_RGB, GL_UNSIGNED_BYTE, pixels);

	delete[] pixels;
	return 0;
}

void GLRenderer::initGL() {
	int link_result = 0;
	ifstream *f;
	f = new ifstream("C:\\Users\\vdao5\\Documents\\cap3dvs\\Debug\\vertex.shader");
	if (f->fail()) {
		throw new string("Vertex shader not found of could not be opened!");
	}
	string shader_src;
	f->seekg(0, ios::end);
	shader_src.reserve(f->tellg());
	f->seekg(0, ios::beg);
	shader_src.assign((std::istreambuf_iterator<char>(*f)), std::istreambuf_iterator<char>());
	f->close();
	delete f;

	this->vertexShaderId = createShader(GL_VERTEX_SHADER, shader_src);

	f = new ifstream("C:\\Users\\vdao5\\Documents\\cap3dvs\\Debug\\fragment.shader");
	if (f->fail()) {
		throw new string("Fragment shader not found of could not be opened!");
	}
	shader_src.clear();
	f->seekg(0, ios::end);
	shader_src.reserve(f->tellg());
	f->seekg(0, ios::beg);
	shader_src.assign((std::istreambuf_iterator<char>(*f)), std::istreambuf_iterator<char>());
	f->close();
	delete f;

	this->fragmentShaderId = createShader(GL_FRAGMENT_SHADER, shader_src);

	this->programId = glCreateProgram();
	glAttachShader(this->programId, this->vertexShaderId);
	glAttachShader(this->programId, this->fragmentShaderId);

	glLinkProgram(this->programId);
	glGetProgramiv(this->programId, GL_LINK_STATUS, &link_result);
	if (link_result == GL_FALSE) {
		throw new string("Link GL program failed!!!!");
	}
}