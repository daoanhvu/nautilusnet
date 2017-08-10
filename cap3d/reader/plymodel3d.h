#ifndef _PLYMODEL3D_H_
#define _PLYMODEL3D_H_

#include <vector>
#include <iostream>
#include "model3d.h"

using namespace std;

class PLYModel3D: public Model3D {
  protected:
    vector<Face> faces;

  public:
    PLYModel3D():Model3D() {};
    PLYModel3D(vector<Vertex>& vs, vector<Face>& f, int floatStride): Model3D(vs, floatStride), faces(f) {
    }

    virtual ~PLYModel3D() {
    	unsigned int size = vertices.size();
		unsigned int i;
		for(i=0; i<size; i++) {
			delete[] vertices[i].face_indices;
		}
	}

	void rotate(float rad, float vx, float vy, float vz);
	void scale(float scale);
	int add_normal_vectors();

    void setFloatStride(int fs) {
      this->float_stride = fs;
    }

    int copyVerticesFrom(vector<Vertex>& srcV) {
      vertices = srcV;
      return vertices.size();
    }

    void reserveVertices(int vertex_count) {
      vertices.reserve(vertex_count);
    }

    void pushVertex(Vertex& v) {
      vertices.push_back(v);
    }

    void reserveFaces(int face_count) {
      faces.reserve(face_count);
    }

    int getFaceCount() const {
      return faces.size();
    }

    Face getFace(int idx) const {
      return faces[idx];
    }

    void pushFace(Face& f) {
      faces.push_back(f);
    }

    void setAll(vector<Vertex> vs, vector<Face> f, int floatStride) {
      vertices = vs;
      faces = f;
      float_stride = floatStride;
    }

    /*
			This will make the performance down
			Need const ???
	*/
	Vertex getVertex(int idx) const {
		return vertices[idx];
	}

	/*
		TODO: This function will fail if the number of vertex per face is not a constant
		PARAMS:
			[OUT] nc: total number of index
	*/
	unsigned int *getElementIndices(unsigned int &nc) {
		unsigned int face_count = faces.size();
		unsigned int *indices = NULL;

		nc = 0;
		if(face_count > 0) {
			int c = 0;
			int num_vertex_per_face;
			num_vertex_per_face = faces[0].vertex_count;
			nc = face_count * num_vertex_per_face;
			// std::cout << "[DEBUG] Number of index: " << nc << endl;
			indices = new unsigned int[nc];
			for(int i=0; i<face_count; i++) {

				// std::cout << "[DEBUG] Face["<< i <<"] " << faces[i].vertex_indices[0] << " " << faces[i].vertex_indices[1] << " " << faces[i].vertex_indices[2] << endl;

				for(int j=0; j<num_vertex_per_face; j++) {
					indices[c++] = faces[i].vertex_indices[j];
				}
			}
		}

		return indices;
	}

	void addDefaultColor(float r, float g, float b) {
		int old_float_stride = float_stride;
		float_stride += 3;
		float *temp_p, *new_v;
		int vxcount = vertices.size();
		for(int i=0; i<vxcount; i++) {
			temp_p = vertices[i].v;
			new_v = new float[float_stride];
			memcpy(new_v, temp_p, sizeof(float) * old_float_stride);
			new_v[old_float_stride] = r;
			new_v[old_float_stride + 1] = g;
			new_v[old_float_stride + 2] = b;
			vertices[i].v = new_v;
			delete[] temp_p;
		}
		VertexAttrib color_att;
		color_att.code = COLOR3;
		color_att.offset = old_float_stride;
		// cout << "[DEBUG] Color offset: " << old_float_stride << endl;
		vertex_attribs.push_back(color_att);
	}

    /*
		For testing
	*/
	void print(ostream &out) {
		int i, j, k;
		int vertex_count = vertices.size();
		int face_count = faces.size();

		int offset = getPositionOffset();
		cout << "[DEBUG] Position offset: " << offset << endl;
		offset = getNormalOffset();
		cout << "[DEBUG] Normal offset: " << offset << endl;
		offset = getColorOffset();
		cout << "[DEBUG] Color offset: " << offset << endl;


		out << "Number of vertex: " << vertex_count << endl;
		out << "Number of faces: " << face_count << endl;
		out << "Number of float per vertex: " << float_stride << endl;
		out << "Vertices: " << endl;
		for(i=0; i<vertex_count; i++) {
			for(j=0; j<float_stride; j++)
				out << vertices[i].v[j] << " ";
			out << endl;
		}

		for(i=0; i<face_count; i++) {
			k = faces[i].vertex_count;
			out << k << " ";
			for(j=0; j<k; j++)
				out << faces[i].vertex_indices[j] << " ";
			out << endl;
		}
	}
};

#endif
