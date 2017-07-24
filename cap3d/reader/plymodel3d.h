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
    PLYModel3D(){};
    PLYModel3D(vector<Vertex> vs, vector<Face> f, int floatStride) {
      vertices = vs;
      faces = f;
      float_stride = floatStride;
    }

    virtual ~PLYModel3D() {
    	unsigned int size = vertices.size();
		unsigned int i;
		for(i=0; i<size; i++) {
			delete[] vertices[i].face_indices;
		}
	}

		void translate(float vx, float vy, float vz);
		void rotate(float rad, float vx, float vy, float vz);
		void scale(float scale);
		void scaleToFit(float value);
		int add_normal_vectors();

    void setFloatStride(int fs) {
      this->float_stride = fs;
    }

    int copyVerticesFrom(vector<Vertex> srcV) {
      vertices = srcV;
      return vertices.size();
    }

    void reserveVertices(int vertex_count) {
      vertices.reserve(vertex_count);
    }

    void pushVertex(Vertex v) {
      vertices.push_back(v);
    }

    void reserveFaces(int face_count) {
      faces.reserve(face_count);
    }

    int getFaceCount() const {
      return faces.size();
    }

    void pushFace(Face f) {
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

    int getFloatStride() const { return float_stride; }

    /*
			Params:
				n [OUT] number of float returned
		*/
		float* getVertexBuffer(unsigned int &);
		float* getNormalBuffer(unsigned int &);

    void getBBox(BBox3d &bbox);

		/*
			TODO: This function will fail if the number of vertex per face is not a constant
			PARAMS:
				[OUT] nc: total number of vertex
		*/
		unsigned short *getElementIndices(unsigned int &nc) {
			unsigned int face_count = faces.size();
			unsigned int i;
			int j, k = faces[0].vertex_count;
			nc = face_count * k;
			int c = 0;
			unsigned short *indices = new unsigned short[nc];
			for(i=0; i<face_count; i++) {
				for(j=0; j<k; j++) {
					indices[c++] = faces[i].vertex_indices[j];
				}
			}
			return indices;
		}

    /*
		 	For testing
		*/
		void print(ostream &out) {
			int i, j, k;
			int vertex_count = vertices.size();
			int face_count = faces.size();

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
