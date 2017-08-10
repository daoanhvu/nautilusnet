#include "plymodel3d.h"
#include <glm/glm.hpp>

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

	cout << "[DEBUG] Number of face: " << face_count << "; Number of vertex: " << vertices.size() << endl;

	//short-circuit
	if(face_count <= 0)
		return 0;

	for(i=0; i<face_count; i++) {
		vxcount = faces[i].vertex_count;
		v = faces[i].vertex_indices;

		// cout << "[DEBUG] face [" << i << "] vertice: " << v[0] << " " << v[1] << " " << v[2] << endl;

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

	// cout << "[DEBUG] GOT HERE 1 " << endl;

	/**
		We are going to add 3 components to vertex, so float_stride need to be increase 3
	*/
	int old_float_stride = float_stride;
	float_stride += 3;
	float *temp_p, *new_v;
	vxcount = vertices.size();
	for(i=0; i<vxcount; i++) {
		normal = glm::vec3(0, 0, 0);
		for(int j=0; j<vertices[i].face_size; j++) {
			normal[0] += faces[vertices[i].face_indices[j]].normal[0];
			normal[1] += faces[vertices[i].face_indices[j]].normal[1];
			normal[2] += faces[vertices[i].face_indices[j]].normal[2];
		}
		normal = glm::normalize(normal);
		temp_p = vertices[i].v;
		
		new_v = new float[float_stride];
		memcpy(new_v, temp_p, sizeof(float) * old_float_stride);

		new_v[old_float_stride] = normal[0];
		new_v[old_float_stride + 1] = normal[1];
		new_v[old_float_stride + 2] = normal[2];

		vertices[i].v = new_v;
		delete[] temp_p;
	}
	VertexAttrib normal_att;
	normal_att.code = NORMAL;
	normal_att.offset = old_float_stride;
	cout << "[DEBUG] Normal offset: " << old_float_stride << endl;
	vertex_attribs.push_back(normal_att);

	return 0;
}
