#ifndef __PLYFILE_H_
#define __PLYFILE_H_

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>

using namespace std;

//define token codes
#define CODE_PLY 			1
#define CODE_PROPERTY 	2
#define CODE_NUMBER 		3
#define CODE_COORD_X 	4
#define CODE_COORD_Y 	5
#define CODE_COORD_Z 	6
#define CODE_FLOAT32 	7
#define CODE_LIST 			8
#define CODE_FORMAT 		9
#define CODE_FACE 			10
#define CODE_END_HEADER 11
#define CODE_INT32 		12
#define CODE_COMMENT 	13
#define CODE_ELEMENT 	14
#define CODE_VERTEX		15
#define CODE_VERTEX_INDICES		16
#define CODE_UNIT32	17
#define CODE_UNIT8	18

typedef struct tagTK {
		int code;

		//In case of code = CODE_NUMBER
		float value;

		tagTK() {
			code = 0;
			value = 0.0f;
		}
		tagTK(int cd) {
			code = cd;
			value = 0.0f;
		}
		tagTK(int cd, float v) {
			code = cd;
			value = v;
		}
} Token;

/*
	This structure stores the information about th co-face that a vertex is
*/
typedef struct tagCoVertex{
	int vertex_index;
	int count; //number of faces this vertex belong to
	int face_indices[32]; //whose indices they are

	tagCoVertex() {
		vertex_index = 0;
		count = 0;
	}

} CoVertex;

typedef struct tagProp {
	string name;
	int datatype;
} PlyProperty;

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
		vector<CoVertex> coVertices;
		float *vertices;
		//Number of float per vertex
		int float_stride;
		unsigned int vertex_count;
		Face* faces;
		unsigned int face_count;

	public:
		PlyFile();
		PlyFile(const char *filename);
		PlyFile(string filename);
		~PlyFile() {
			if(vertices != 0) {
				delete[] vertices;
			}

			if(faces != 0) {
				delete[] faces;
			}
		}

		int load(const char *filename);
		int load2(const char *filename);
		int add_normal_vectors();

		void getBBox(BBox3d &bbox);
		int parse_line2(string line, vector<Token> &v);


		/*
		 	For testing
		*/
		void print(ostream &out) {
			int i, j, k;
			out << "Number of vertex: " << vertex_count << endl;
			out << "Number of faces: " << face_count << endl;
			out << "Vertices: " << endl;
			for(i=0; i<vertex_count; i++) {
				k = i * float_stride;
				for(j=0; j<float_stride; j++)
					out << vertices[k+j] << " ";
				out << endl;

				out << "Faces: ";
				for(j=0; j<coVertices[i].count; j++) {
					out << coVertices[i].face_indices[j] << ", ";
				}
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
