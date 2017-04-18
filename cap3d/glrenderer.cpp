#include <glrenderer.h>
#include <iostream>
#include <fstream>

#include <exception>
#include <cstring>
#include <sstream>

#include <GLFW/glfw3.h>

#include <camera.h>

//#include <glm/gtx/transform.hpp>
#include <glm/gtc/matrix_transform.hpp>
/*
	This method use glReadPixels to capture data from a frame
	http://www.opengl-tutorial.org/beginners-tutorials/tutorial-2-the-first-triangle/

*/

#define RAD(x) (x*3.14159f/180.0f)

using namespace std;
using namespace fp;

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
		int logLen;
		glGetShaderiv(shaderid, GL_INFO_LOG_LENGTH, &logLen);
		if(logLen > 0) {
			char* msg = new char[logLen+1];
			glGetShaderInfoLog(shaderid, logLen, NULL, msg);
			cout << msg << endl;
			delete[] msg;
		}
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

int GLRenderer::initGL(const char* vertexFile, const char* fragmentFile) {
	int link_result = 0;
	ifstream *f;
	//f = new ifstream("C:\\Users\\vdao5\\Documents\\cap3dvs\\Debug\\vertex.shader");
	f = new ifstream(vertexFile);
	if (f->fail()) {
		cout << "Vertex shader not found of could not be opened!" << endl;
		return 1;
		//throw new string("Vertex shader not found of could not be opened!");
	}

	string shader_src;
	f->seekg(0, ios::end);
	shader_src.reserve(f->tellg());
	f->seekg(0, ios::beg);
	shader_src.assign((std::istreambuf_iterator<char>(*f)), std::istreambuf_iterator<char>());
	f->close();
	delete f;

	this->vertexShaderId = createShader(GL_VERTEX_SHADER, shader_src);

	if(this->vertexShaderId == 0) {
		cout << "Create vertex shader failed!!!";
		return 1;
	}

	f = new ifstream(fragmentFile);
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
	if(this->fragmentShaderId == 0) {
		cout << "Create fragment shader failed!!!";
		return 1;
	}

	this->programId = glCreateProgram();
	glAttachShader(this->programId, this->vertexShaderId);
	glAttachShader(this->programId, this->fragmentShaderId);

	glLinkProgram(this->programId);
	glGetProgramiv(this->programId, GL_LINK_STATUS, &link_result);
	if (link_result == GL_FALSE) {
		//throw new string("Link GL program failed!!!!");
		cout << "Link GL Program failed!!!";
		return 1;
	}
	cout << "Renderer init OK!!!";
	return 0;
}

void GLRenderer::moveCameraTo(float ex, float ey, float ez, float cx, float cy, float cz, const PlyFile *model, const char *exportedFile){
	Camera cam;

	int vertex_count = model->getVertexCount();

	cam.setViewport(0, 0, 200, 200);
	cam.lookAt(ex, ey, ez, cx, cy, cz, 0.0f, 1.0f, 0.0f);
	float fov = RAD(45.0f) ;
	cam.setPerspective(fov, 0.1f, 99.0f);

	cv::Vec3b color;
	color[0] = 0;
	color[0] = 0;
	color[0] = 255;

	cv::Mat img(200, 200, CV_8UC3, cv::Scalar(0, 0, 0));
	// float *out = new float[model->properties.size() * model->vertex_count];
	float *out = new float[model->getFloatStride() * model->getVertexCount()];
	// cout << "vertex count: " << model->getVertexCount() << " " << sizeof(model->vertices) <<endl;
	float p[3];
	int i, offs;
	int x, y;
	for(i=0; i<vertex_count; i++) {
		//offs = i * model->properties.size();
		offs = i * 3;
		cam.project(out + offs, model->getVertex(i).v);
		//cout << "x= " << (model->vertices)[offs] << "; y= " << (model->vertices)[offs + 1] << "; z=" << (model->vertices)[offs+2] << std::endl;
		x = (int) out[offs];
		y = (int) out[offs + 1];
		//cout << "xw= " << model->vertices[offs] << ", yw="<< model->vertices[offs+1] << ", zw="<< model->vertices[offs+2] << " x= " << x << "; y= "<< y <<endl;
		if( (x >= 0) && (x<200) && (y >= 0) && (y<200))
			img.at<cv::Vec3b>(x, y) = color;
	}
	cout << "Writing image "<< exportedFile <<"..." <<endl;
	//write image matrice to file
	imwrite(exportedFile, img);
	cout << "Done writing image." <<endl;

	//release memories
	delete[] out;
}
