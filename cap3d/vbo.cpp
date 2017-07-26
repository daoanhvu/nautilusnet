#include "vbo.h"

VBO::VBO(const Model3D *model) {
	int offs = model->getColorOffset();
	hasColor = offs>=0?true:false;

	offs = model->getNormalOffset();
	hasNormal = offs>=0?true:false;
}

