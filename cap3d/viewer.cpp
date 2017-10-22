#include "viewer.h"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/transform.hpp>

Viewer::Viewer():viewCoordinator(true) {

}

void Viewer::setupCamera(glm::vec3 cam_pos, glm::vec3 eye_center) {
	this->projectionMatrix = glm::perspective(glm::radians(45.0f), 4.0f / 3.0f, 0.1f, 100.0f);
	this->viewMatrix = glm::lookAt(
			cam_pos,
			eye_center,
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
	coordinatorVBO.setup(v, vc, float_stride, NULL, 0, this->shaderVarLocation);
}

void Viewer::drawCoordinator() {
	//TODO: Turn off all lights
	coordinatorVBO.draw(this->shaderVarLocation, this->projectionMatrix, this->viewMatrix);
	//TODO: Turn on all lights
}

void Viewer::drawScene() {
	int i;
	if(viewCoordinator) {
		drawCoordinator();
	}

	for(i=0; i<models.size(); i++) {
		models[i]->draw(this->shaderVarLocation, this->projectionMatrix, this->viewMatrix);
	}
}