#include "viewer.h"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/transform.hpp>

Viewer::Viewer():viewCoordinator(true) {

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