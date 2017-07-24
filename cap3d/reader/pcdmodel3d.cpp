#include "pcdmodel3d.h"
#include <glm/glm.hpp>

float* PCDModel3D::getVertexBuffer(unsigned int &nc) {
	int i, j;
	nc = vertices.size();
	unsigned int n = nc * float_stride;
	float *buf = new float[n];
	int offs = 0;
	unsigned int normal_size_in_byte = sizeof(float) * float_stride;
	for(i=0; i<nc; i++) {
		std::memcpy(buf + offs, vertices[i].v, normal_size_in_byte);
		offs += float_stride;
	}
	return buf;
}

void PCDModel3D::translate(float vx, float vy, float vz) {
	unsigned int nc = vertices.size();
	unsigned int i;

	for(i=0; i<nc; i++) {
		vertices[i].v[0] += vx;
		vertices[i].v[1] += vy;
		vertices[i].v[2] += vz;
	}
}

//TODO: Not implemented yet
void PCDModel3D::rotate(float rad, float vx, float vy, float vz) {

}

void PCDModel3D::scale(float scale) {
	unsigned int nc = vertices.size();
	unsigned int i;

	for(i=0; i<nc; i++) {
		vertices[i].v[0] *= scale;
		vertices[i].v[1] *= scale;
		vertices[i].v[2] *= scale;
	}
}

void PCDModel3D::scaleToFit(float value) {
	float maxx = -9999.0f;
	int i;
	int vertex_count = vertices.size();

	for(i=0; i<vertex_count; i++) {
		if(vertices[i].v[0] > maxx) {
			maxx = vertices[i].v[0];
		}

		if(vertices[i].v[1] > maxx) {
			maxx = vertices[i].v[1];
		}

		if(vertices[i].v[2] > maxx) {
			maxx = vertices[i].v[2];
		}
	}
	float factor = value / maxx;
	for(i=0; i<vertex_count; i++) {
		vertices[i].v[0] *= factor;
		vertices[i].v[1] *= factor;
		vertices[i].v[2] *= factor;
	}
}
