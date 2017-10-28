#include "viewer.h"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/transform.hpp>

Viewer::Viewer():viewCoordinator(true) {
	rotationMatrix = glm::mat4(1.0f);
}

void Viewer::setupCamera(int viewWidth, int viewHeight, glm::vec3 campos, glm::vec3 ec) {
	this->eye_center = ec;
	this->cam_pos = campos;
	this->projectionMatrix = glm::perspective(glm::radians(45.0f), (viewWidth * 1.0f) / viewHeight, 0.1f, 1000.0f);
	this->viewMatrix = glm::lookAt(
			this->cam_pos,
			this->eye_center,
			glm::vec3(0, 1, 0)
		);
}
	
void Viewer::setup(std::vector<Model3D*> m){
	int size = m.size();
	VBO *v;

	for(int i=0; i<size; i++) {
		v = new VBO(GL_TRIANGLES, GL_STATIC_DRAW);
		v->setup(m[i], this->shaderVarLocation);
		this->models.push_back(v);
	}
}

void Viewer::addModel(const Model3D *model) {
	VBO *v = new VBO(GL_TRIANGLES, GL_STATIC_DRAW);
	v->setup(model, this->shaderVarLocation);
	this->models.push_back(v);
}

void Viewer::setupCoordinator() {
	float v[] = {	
		0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 	1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f,
		0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 	0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f,
		0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 	0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f
	};
	int vc = 6;
	int float_stride = 6;
	coordinatorVBO.setDrawPrimitive(GL_LINES);
	coordinatorVBO.setup(v, vc, float_stride, 0, 12 /* 3*sizeof(float)*/, -1, -1,
		NULL, 0, this->shaderVarLocation);

	//for testing purpose
	setupViewpointsArroundObject();
}

void Viewer::drawCoordinator() {
	//TODO: Turn off all lights
	coordinatorVBO.draw(this->shaderVarLocation, this->projectionMatrix, this->viewMatrix);
	//TODO: Turn on all lights
}

void Viewer::drawScene() {
	int i;
	if(viewCoordinator) {
		coordinatorVBO.draw(this->shaderVarLocation, this->projectionMatrix, this->viewMatrix);
	}

	for(i=0; i<models.size(); i++) {
		models[i]->draw(this->shaderVarLocation, this->projectionMatrix, this->viewMatrix);
	}
}

void Viewer::setupViewpointsArroundObject() {
	float v[] = {
		-0.1f, 0.1f, 1.0f, 	1.0f, 0.7f, 0.4f,
		0.1f, 0.1f, 1.0f, 	1.0f, 0.7f, 0.4f,
		0.1f, 0.0f, 1.0f, 		1.0f, 0.7f, 0.4f,
		-0.1f, 0.0f, 1.0f, 	1.0f, 0.7f, 0.4f
	};
	float PI_12 = 30 * 3.14159265f / 180.0f;
	unsigned int idx[] = {0, 3, 1, 2};
	int vc = 4;
	int float_stride = 6;
	VBO *vbo = new VBO(GL_TRIANGLE_STRIP, GL_STATIC_DRAW);
	vbo->setup(v, vc, float_stride, 0, 12, -1, -1, idx, 4, this->shaderVarLocation);
	// vbo->rotate(0.0f, glm::vec3(0, 1, 0));
	this->models.push_back(vbo);

	//Virtual camera 2
	vbo = new VBO(GL_TRIANGLE_STRIP, GL_STATIC_DRAW);
	vbo->setup(v, vc, float_stride, 0, 12, -1, -1, idx, 4, this->shaderVarLocation);
	vbo->rotate(PI_12, glm::vec3(0, 1, 0));
	this->models.push_back(vbo);

	//Virtual camera 3
	vbo = new VBO(GL_TRIANGLE_STRIP, GL_STATIC_DRAW);
	vbo->setup(v, vc, float_stride, 0, 12, -1, -1, idx, 4, this->shaderVarLocation);
	vbo->rotate(2*PI_12, glm::vec3(0, 1, 0));
	this->models.push_back(vbo);

	//Virtual camera 4
	vbo = new VBO(GL_TRIANGLE_STRIP, GL_STATIC_DRAW);
	vbo->setup(v, vc, float_stride, 0, 12, -1, -1, idx, 4, this->shaderVarLocation);
	vbo->rotate(3*PI_12, glm::vec3(0, 1, 0));
	this->models.push_back(vbo);

	//Virtual camera 5
	vbo = new VBO(GL_TRIANGLE_STRIP, GL_STATIC_DRAW);
	vbo->setup(v, vc, float_stride, 0, 12, -1, -1, idx, 4, this->shaderVarLocation);
	vbo->rotate(4*PI_12, glm::vec3(0, 1, 0));
	this->models.push_back(vbo);

	//Virtual camera 6
	vbo = new VBO(GL_TRIANGLE_STRIP, GL_STATIC_DRAW);
	vbo->setup(v, vc, float_stride, 0, 12, -1, -1, idx, 4, this->shaderVarLocation);
	vbo->rotate(5*PI_12, glm::vec3(0, 1, 0));
	this->models.push_back(vbo);

	//Virtual camera 7
	vbo = new VBO(GL_TRIANGLE_STRIP, GL_STATIC_DRAW);
	vbo->setup(v, vc, float_stride, 0, 12, -1, -1, idx, 4, this->shaderVarLocation);
	vbo->rotate(6*PI_12, glm::vec3(0, 1, 0));
	this->models.push_back(vbo);

	//Virtual camera 8
	vbo = new VBO(GL_TRIANGLE_STRIP, GL_STATIC_DRAW);
	vbo->setup(v, vc, float_stride, 0, 12, -1, -1, idx, 4, this->shaderVarLocation);
	vbo->rotate(7*PI_12, glm::vec3(0, 1, 0));
	this->models.push_back(vbo);

	//Virtual camera 9
	vbo = new VBO(GL_TRIANGLE_STRIP, GL_STATIC_DRAW);
	vbo->setup(v, vc, float_stride, 0, 12, -1, -1, idx, 4, this->shaderVarLocation);
	vbo->rotate(8*PI_12, glm::vec3(0, 1, 0));
	this->models.push_back(vbo);

	//Virtual camera 10
	vbo = new VBO(GL_TRIANGLE_STRIP, GL_STATIC_DRAW);
	vbo->setup(v, vc, float_stride, 0, 12, -1, -1, idx, 4, this->shaderVarLocation);
	vbo->rotate(9*PI_12, glm::vec3(0, 1, 0));
	this->models.push_back(vbo);

	//Virtual camera 11
	vbo = new VBO(GL_TRIANGLE_STRIP, GL_STATIC_DRAW);
	vbo->setup(v, vc, float_stride, 0, 12, -1, -1, idx, 4, this->shaderVarLocation);
	vbo->rotate(10*PI_12, glm::vec3(0, 1, 0));
	this->models.push_back(vbo);

	//Virtual camera 12
	vbo = new VBO(GL_TRIANGLE_STRIP, GL_STATIC_DRAW);
	vbo->setup(v, vc, float_stride, 0, 12, -1, -1, idx, 4, this->shaderVarLocation);
	vbo->rotate(11*PI_12, glm::vec3(0, 1, 0));
	this->models.push_back(vbo);
}