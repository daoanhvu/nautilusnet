#ifndef _MODEL3D_H_
#define _MODEL3D_H_

#include <vector>
#include <iostream>
#include "../common_cap3d.h"

using namespace std;

typedef struct tagPCDVertex {
	float *v;
	int length;

	tagPCDVertex() {
		v = NULL;
		length = 0;
	}
} PCDVertex;

class PCDModel3D {
  protected:
    vector<PCDVertex> vertices;
    //Number of float per vertex
    int float_stride;

  public:
    PCDModel3D(){};
    PCDModel3D(vector<PCDVertex> vs, int floatStride) {
      vertices = vs;
      float_stride = floatStride;
    }

    virtual ~PCDModel3D() {
			unsigned int size = vertices.size();
			unsigned int i, j;

			for(i=0; i<size; i++) {
				delete[] vertices[i].v;
			}
		}

	void translate(float vx, float vy, float vz);
	void rotate(float rad, float vx, float vy, float vz);
	void scale(float scale);
	void scaleToFit(float value);

    void setFloatStride(int fs) {
      this->float_stride = fs;
    }

    int copyVerticesFrom(vector<PCDVertex> srcV) {
      vertices = srcV;
      return vertices.size();
    }

    void reserveVertices(int vertex_count) {
      vertices.reserve(vertex_count);
    }

    void pushVertex(PCDVertex v) {
    	vertices.push_back(v);
    }

    int getVertexCount() const {
		return vertices.size();
	}

    void setAll(vector<PCDVertex> vs, int floatStride) {
      vertices = vs;
      float_stride = floatStride;
    }

    /*
			This will make the performance down
			Need const ???
		*/
	PCDVertex getVertex(int idx) const {
		return vertices[idx];
	}

    int getFloatStride() const { return float_stride; }

    /*
			Params:
				n [OUT] number of float returned
		*/
		float* getVertexBuffer(unsigned int &);
		float* getNormalBuffer(unsigned int &);

    // void getBBox(BBox3d &bbox);

    /*
		 	For testing
		*/
		void print(ostream &out) {
			int i, j, k;
			int vertex_count = vertices.size();

			out << "Number of vertex: " << vertex_count << endl;
			out << "Number of float per vertex: " << float_stride << endl;
			out << "Vertices: " << endl;
			for(i=0; i<vertex_count; i++) {
				for(j=0; j<float_stride; j++)
					out << vertices[i].v[j] << " ";
				out << endl;
			}
		}
};

#endif
