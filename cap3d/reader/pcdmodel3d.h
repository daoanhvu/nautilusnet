#ifndef _PCDMODEL3D_H_
#define _PCDMODEL3D_H_

#include <vector>
#include <iostream>
#include "model3d.h"

using namespace std;



class PCDModel3D: public Model3D {
  protected:
  public:
    PCDModel3D(){};
    PCDModel3D(vector<Vertex> vs, int floatStride): Model3D(vs, floatStride) {
    }

    virtual ~PCDModel3D() {		
	}

	void translate(float vx, float vy, float vz);
	void rotate(float rad, float vx, float vy, float vz);
	void scale(float scale);
	void scaleToFit(float value);

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

    void setAll(vector<Vertex> vs, int floatStride) {
      vertices = vs;
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
				[OUT] nc: total number of vertex
		*/
    virtual unsigned short *getElementIndices(unsigned int &nc) {
    	nc = 0;
    	return NULL;
    }

    // void getBBox(BBox3d &bbox);

    /*
	 	For testing
	*/
	void print(ostream &out) {
		int i, j;
		int vertex_count = vertices.size();
		out << "Number of vertex: " << vertex_count << endl;
		out << "Number of float per vertex: " << float_stride << endl;
		out << "Number of component: " << vertex_attribs.size() << endl;
		out << "Vertices: " << endl;
		for(i=0; i<vertex_count; i++) {
			for(j=0; j<float_stride; j++)
				out << vertices[i].v[j] << " ";
			out << endl;
		}
	}
};

#endif
