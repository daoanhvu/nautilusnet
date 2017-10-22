#include "model3d.h"
#include <glm/glm.hpp>

float* Model3D::getVertexBuffer(unsigned int &nc) const {
	int num_of_vertex = vertices.size();
	nc = num_of_vertex * float_stride;
	float *buf = new float[nc];
	unsigned int offs = 0;
	unsigned int v_size_in_byte = sizeof(float) * float_stride;
	for(int i=0; i<num_of_vertex; i++) {

		std::memcpy(buf + offs, vertices[i].v, v_size_in_byte);
		offs += float_stride;
	}
	return buf;
}

/*
	Params
	Return
*/
float* Model3D::getNormalBuffer(unsigned int &nc) {
	int i, j;
	nc = vertices.size();
	unsigned int n = nc * 3;
	float *buf = new float[n];
	int offs = 0;
	int normal_offset = getNormalOffset();
	unsigned int normal_size_in_byte = sizeof(float) * 3;
	for(i=0; i<nc; i++) {
		std::memcpy(buf + offs, vertices[i].v + normal_offset, normal_size_in_byte);
		offs += 3;
	}
	return buf;
}

void Model3D::translate(float vx, float vy, float vz) {
	int nc = vertices.size();
	for(int i=0; i<nc; i++) {
		vertices[i].v[0] += vx;
		vertices[i].v[1] += vy;
		vertices[i].v[2] += vz;
	}
}

//TODO: Not implemented yet
void Model3D::rotate(float rad, float vx, float vy, float vz) {

}

void Model3D::scale(float scale) {
	unsigned int nc = vertices.size();
	unsigned int i;

	for(i=0; i<nc; i++) {
		vertices[i].v[0] *= scale;
		vertices[i].v[1] *= scale;
		vertices[i].v[2] *= scale;
	}
}

void Model3D::scaleToFit(float value) {
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
	// cout << "Scale factor: " << factor << endl;
	for(i=0; i<vertex_count; i++) {
		vertices[i].v[0] *= factor;
		vertices[i].v[1] *= factor;
		vertices[i].v[2] *= factor;
	}
}

void Model3D::getBBox(BBox3d &bbox) {
	bbox.minx = 999999.0f;
	bbox.maxx = -9999.0f;
	bbox.miny = 999999.0f;
	bbox.maxy = -9999.0f;
	bbox.minz = 999999.0f;
	bbox.maxz = -9999.0f;

	int vertex_count = vertices.size();

	for(int i=0; i<vertex_count; i++) {
		if(vertices[i].v[0] < bbox.minx) {
			bbox.minx = vertices[i].v[0];
		}

		if(vertices[i].v[0] > bbox.maxx) {
			bbox.maxx = vertices[i].v[0];
		}

		if(vertices[i].v[1] < bbox.miny) {
			bbox.miny = vertices[i].v[1];
		}

		if(vertices[i].v[1] > bbox.maxy) {
			bbox.maxy = vertices[i].v[1];
		}

		if(vertices[i].v[2] < bbox.minz) {
			bbox.minz = vertices[i].v[2];
		}

		if(vertices[i].v[2] > bbox.maxz) {
			bbox.maxz = vertices[i].v[2];
		}
	}
}
