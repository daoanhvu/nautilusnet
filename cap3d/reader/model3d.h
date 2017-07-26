#ifndef _MODEL3D_H_
#define _MODEL3D_H_

#include <vector>
#include <iostream>

using namespace std;

typedef struct tagVertex {
	float *v;
	float normal[3];
	int *face_indices;
	unsigned int count; //number of faces this vertex belong to
	unsigned int log_face_size;

public:
	tagVertex() {
		v = NULL;
		count = 0;
		// face_indices = new int[8];
		face_indices = NULL;
		log_face_size = 0;
	}

	tagVertex(const struct tagVertex &vt) {
		this->v = vt.v;
		this->count = vt.count;
		this->face_indices = vt.face_indices;
		this->log_face_size = vt.log_face_size;
		this->normal[0] = vt.normal[0];
		this->normal[1] = vt.normal[1];
		this->normal[2] = vt.normal[2];
	}

	struct tagVertex& operator=(const struct tagVertex& v) {
		this->v = v.v;
		this->count = v.count;
		this->face_indices = v.face_indices;
		this->log_face_size = v.log_face_size;
		this->normal[0] = v.normal[0];
		this->normal[1] = v.normal[1];
		this->normal[2] = v.normal[2];
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
    //Number of float per vertex
    int float_stride;
	vector<VertexAttrib> vertex_attribs;

  public:
    Model3D(): bufferCount(0) {}
    Model3D(vector<Vertex> vs, int floatStride): bufferCount(0) {
    	vertices = vs;
    	float_stride = floatStride;
    }

    virtual ~Model3D() {
		unsigned int size = vertices.size();
		unsigned int i;

		for(i=0; i<size; i++) {
			delete[] vertices[i].v;
		}
	}

	void translate(float vx, float vy, float vz);
	void rotate(float rad, float vx, float vy, float vz);
	void scale(float scale);
	void scaleToFit(float value);

	virtual void addAttrib(VertexAttrib att) {
		vertex_attribs.push_back(att);
	}

	virtual short getColorOffset() {
		for(int i=0; i<vertex_attribs.size(); i++) {
			if(vertex_attribs[i].code == COLOR3 || vertex_attribs[i].code == COLOR4) {
				return vertex_attribs[i].offset;
			}
		}
		return -1;
	}

	virtual int getNormalOffset() {
		for(int i=0; i<vertex_attribs.size(); i++) {
			if(vertex_attribs[i].code == NORMAL) {
				return vertex_attribs[i].offset;
			}
		}
		return -1;
	}

	virtual int getTextureOffset() {
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

    int getVertexCount() const {
		return vertices.size();
	}

    virtual void setAll(vector<Vertex> vs, int floatStride) {
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

    int getFloatStride() const { return float_stride; }

    /*
		Params:
		n [OUT] number of float returned
	*/
	float* getVertexBuffer(unsigned int &);
	float* getNormalBuffer(unsigned int &);

    virtual void getBBox(BBox3d &bbox);

    /*
			TODO: This function will fail if the number of vertex per face is not a constant
			PARAMS:
				[OUT] nc: total number of vertex
		*/
    virtual unsigned short *getElementIndices(unsigned int &nc) = 0;

    virtual void draw() = 0;
};

#endif
