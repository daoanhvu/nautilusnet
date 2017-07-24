#include "plymodel3d.h"
#include <glm/glm.hpp>

float* PLYModel3D::getVertexBuffer(unsigned int &nc) {
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

/*
	Params
	Return
*/
float* PLYModel3D::getNormalBuffer(unsigned int &nc) {
	int i, j;
	nc = vertices.size();
	unsigned int n = nc * 3;
	float *buf = new float[n];
	int offs = 0;
	unsigned int normal_size_in_byte = sizeof(float) * 3;
	for(i=0; i<nc; i++) {
		std::memcpy(buf + offs, vertices[i].normal, normal_size_in_byte);
		offs += 3;
	}
	return buf;
}

void PLYModel3D::translate(float vx, float vy, float vz) {
	unsigned int nc = vertices.size();
	unsigned int i;

	for(i=0; i<nc; i++) {
		vertices[i].v[0] += vx;
		vertices[i].v[1] += vy;
		vertices[i].v[2] += vz;
	}
}

//TODO: Not implemented yet
void PLYModel3D::rotate(float rad, float vx, float vy, float vz) {

}

void PLYModel3D::scale(float scale) {
	unsigned int nc = vertices.size();
	unsigned int i;

	for(i=0; i<nc; i++) {
		vertices[i].v[0] *= scale;
		vertices[i].v[1] *= scale;
		vertices[i].v[2] *= scale;
	}
}

void PLYModel3D::scaleToFit(float value) {
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

/*
	Calculate normal vector for faces
*/
int PLYModel3D::add_normal_vectors() {
	int i;
	int vxcount;
	int *v;
	glm::vec3 e1, e2;
	glm::vec3 normal;
	float v0a, v0b, v0c;
	float v1a, v1b, v1c;
	float v2a, v2b, v2c;
	int face_count = faces.size();

	for(i=0; i<face_count; i++) {
		vxcount = faces[i].vertex_count;
		v = faces[i].vertex_indices;

		v0a = vertices[v[0]].v[0];
		v0b = vertices[v[0]].v[1];
		v0c = vertices[v[0]].v[2];

		v1a = vertices[v[1]].v[0];
		v1b = vertices[v[1]].v[1];
		v1c = vertices[v[1]].v[2];

		v2a = vertices[v[2]].v[0];
		v2b = vertices[v[2]].v[1];
		v2c = vertices[v[2]].v[2];

		e1 = glm::vec3(v1a - v0a, v1b - v0b, v1c - v0c);
		e2 = glm::vec3(v2a - v0a, v2b - v0b, v2c - v0c);

		normal = glm::cross(e1, e2);
		normal = glm::normalize(normal);
		faces[i].normal[0] = normal[0];
		faces[i].normal[1] = normal[1];
		faces[i].normal[2] = normal[2];
		//cout << "Normal vector: " << normal[0] << ", " << normal[1] << ", " << normal[2] << endl;
	}

	vxcount = vertices.size();
	for(i=0; i<vxcount; i++) {
		normal = glm::vec3(0, 0, 0);
		for(int j=0; j<vertices[i].count; j++) {
			normal[0] += faces[vertices[i].face_indices[j]].normal[0];
			normal[1] += faces[vertices[i].face_indices[j]].normal[1];
			normal[2] += faces[vertices[i].face_indices[j]].normal[2];
		}
		normal = glm::normalize(normal);
		vertices[i].normal[0] = normal[0];
		vertices[i].normal[1] = normal[1];
		vertices[i].normal[2] = normal[2];
	}
	return 0;
}

void PLYModel3D::getBBox(BBox3d &bbox) {
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
