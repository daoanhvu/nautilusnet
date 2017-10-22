#ifndef _MODEL3D_H_
#define _MODEL3D_H_

#include <vector>
#include <iostream>

#ifdef __linux__
	#include <cstring>
#endif

using namespace std;

/**
	deprecated!!!!
*/
typedef struct tagVertex {
	float *v;
	int *face_indices;
	unsigned int face_size; //number of faces this vertex belong to
	unsigned int log_face_size;

public:
	tagVertex() {
		v = NULL;
		face_size = 0;
		// face_indices = new int[8];
		face_indices = NULL;
		log_face_size = 0;
	}

	tagVertex(const struct tagVertex &vt) {
		this->v = vt.v;
		this->face_size = vt.face_size;
		this->face_indices = vt.face_indices;
		this->log_face_size = vt.log_face_size;
	}

	struct tagVertex& operator=(const struct tagVertex& v) {
		this->v = v.v;
		this->face_size = v.face_size;
		this->face_indices = v.face_indices;
		this->log_face_size = v.log_face_size;
		return *this;
	}
} Vertex;

typedef struct tagFace {
	unsigned char vertex_count;
	int vertex_indices[5];
	float normal[3];
} Face;

typedef struct tagBBox3D {
	float minx;
	float maxx;
	float miny;
	float maxy;
	float minz;
	float maxz;
} BBox3d;

enum VertexAttribute {
	POSITION = 0,
	COLOR3,
	COLOR4,
	NORMAL,
	TEXTURE
};

typedef struct tagVertexAttrib {
	//	0. Position
	//	1. Color
	//	2. Normal
	//	3. Texture
	VertexAttribute code;
	short offset;
} VertexAttrib;

class Model3D {
  protected:
    vector<Vertex> vertices;
    //Number of float per each vertex
    int float_stride;
	vector<VertexAttrib> vertex_attribs;

  public:
    Model3D(): float_stride(0) {    }
    Model3D(vector<Vertex>& vs, int floatStride): float_stride(floatStride), vertices(vs) {
    }

    virtual ~Model3D() {
		unsigned int size = vertices.size();
		unsigned int i;

		for(i=0; i<size; i++) {
			delete[] vertices[i].v;
		}
	}

	int getAttribCount() const {
		return vertex_attribs.size();
	}

	// int getComponentSize() {
	// 	int size = 0;
	// 	int attr_count = vertex_attribs.size();
	// 	VertexAttribute attr_code;

	// 	for(int i=0; i<attr_count; i++) {
	// 		attr_code = vertex_attribs[i].code;
	// 		switch(attr_code) {
	// 			case POSITION:
	// 			case NORMAL:
	// 			case COLOR3:
	// 				size += 3;
	// 			break;

	// 			case COLOR4:
	// 				size += 4;
	// 			break;

	// 			case TEXTURE:
	// 				size += 2;
	// 			break;
	// 		}
	// 	}

	// 	return size;
	// }

	VertexAttrib getAttrib(int idx) const{
		return vertex_attribs[idx];
	}

	int getAttributeIndex(VertexAttribute code) {
		int i;
		for(i=0; i<vertex_attribs.size(); i++) {
			if(vertex_attribs[i].code == code)
				return i;
		}
		return -1;
	}
	
	void setAttributeByIndex(int idx, VertexAttribute code) {
		vertex_attribs[idx].code = code;
	}

	void translate(float vx, float vy, float vz);
	void rotate(float rad, float vx, float vy, float vz);
	void scale(float scale);
	void scaleToFit(float value);

	virtual void addAttrib(VertexAttrib att) {
		vertex_attribs.push_back(att);
	}

	virtual short getPositionOffset() const {
		for(int i=0; i<vertex_attribs.size(); i++) {
			if(vertex_attribs[i].code == POSITION) {
				return vertex_attribs[i].offset;
			}
		}
		return -1;
	}

	virtual short getColorOffset() const {
		for(int i=0; i<vertex_attribs.size(); i++) {
			if(vertex_attribs[i].code == COLOR3 || vertex_attribs[i].code == COLOR4) {
				return vertex_attribs[i].offset;
			}
		}
		return -1;
	}

	virtual int getNormalOffset() const {
		for(int i=0; i<vertex_attribs.size(); i++) {
			if(vertex_attribs[i].code == NORMAL) {
				return vertex_attribs[i].offset;
			}
		}
		return -1;
	}

	virtual int getTextureOffset() const {
		for(int i=0; i<vertex_attribs.size(); i++) {
			if(vertex_attribs[i].code == TEXTURE) {
				return vertex_attribs[i].offset;
			}
		}
		return -1;
	}

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

    void pushVertex(Vertex v) {
      vertices.push_back(v);
    }

    int getVertexCount() const {
		return vertices.size();
	}

    virtual void setAll(vector<Vertex> vs, int floatStride) {
      vertices = vs;
      float_stride = floatStride;
    }

    /*
		This will make the performance down, use this for DEBUG only!!!
		Need const ???
	*/
	Vertex getVertex(int idx) const {
		return vertices[idx];
	}

    virtual int getFloatStride() const { return float_stride; }

    /*
		Params:
		n [OUT] number of float returned
	*/
	float* getVertexBuffer(unsigned int &) const;
	virtual float* getNormalBuffer(unsigned int &);

    virtual void getBBox(BBox3d &bbox);

    /*
			TODO: This function will fail if the number of vertex per face is not a constant
			PARAMS:
				[OUT] nc: total number of vertex
		*/
    virtual unsigned int *getElementIndices(unsigned int &nc) const = 0;


    //For Debuging
    virtual void print(ostream &out) {}
};

#endif
