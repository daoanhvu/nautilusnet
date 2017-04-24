#ifndef __PLYFILE_H_
#define __PLYFILE_H_

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include "lexer_parser.h"

using namespace std;

typedef struct tagProp {
	string name;
	int datatype;
} PlyProperty;

typedef struct tagVertex {
	float *v;
	float normal[3];
	int *face_indices;
	unsigned int count; //number of faces this vertex belong to
	unsigned int log_face_size;

	tagVertex() {
			v = NULL;
			count = 0;
			face_indices = new int[8];
			log_face_size = 8;
	}
} Vertex;

typedef struct tagFace {
	unsigned char vertex_count;
	int vertex_indices[5];
	float normal[3];
} Face;

struct OpenFileException : public exception {
   const char * what () const throw () {
      return "Could not open this file.";
   }
};

typedef struct tagBBox3D {
	float minx;
	float maxx;
	float miny;
	float maxy;
	float minz;
	float maxz;
} BBox3d;

class PlyFile {
	public:
		vector<PlyProperty> properties;
		vector<Vertex> vertices;
		//Number of float per vertex
		int float_stride;
		vector<Face> faces;

	public:
		PlyFile();
		PlyFile(const char *filename);
		PlyFile(string filename);

		~PlyFile() {
			unsigned int size = vertices.size();
			unsigned int fsize = faces.size();
			unsigned int i, j;

			for(i=0; i<size; i++) {
				delete[] vertices[i].v;
				delete[] vertices[i].face_indices;
			}

		}

		int getVertexCount() const {
			return vertices.size();
		}

		int getFloatStride() const { return float_stride; }

		/*
			This will make the performance down

			Need const ???
		*/
		Vertex getVertex(int idx) const {
			return vertices[idx];
		}

		int load(const char *filename, float scale);
		/*
			Load without scaling
		*/
		int load(const char *filename);
		void scale(float scale);
		int add_normal_vectors();

		void getBBox(BBox3d &bbox);
		int parse_line2(string line, vector<Token> &v);
		/*
			Params:
				n [OUT] number of float returned
		*/
		float* getVertexBuffer(unsigned int &);

		float* getNormalBuffer(unsigned int &);

		/*
			TODO: This function will fail if the number of vertex per face is not a constant
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
