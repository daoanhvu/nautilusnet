#include "viewer.h"


Viewer::Viewer():viewCoordinator(true) {

}
	
void Viewer::setup() {

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
	coordinatorVBO.draw();
	//TODO: Turn on all lights
}

void Viewer::drawScene() {
	int i;
	if(viewCoordinator) {
		drawCoordinator();
	}

	for(i=0; i<models.size(); i++) {
		models[i].vbo->draw();
	}

}