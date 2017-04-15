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

typedef struct tagProp {
	string name;
	int datatype;
} PlyProperty;

typedef struct tagFace {
	unsigned char vertex_count;
	int vertex_indices[5];
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
		int add_normal_vectors();

		void parse_line(string line, vector<Token> &v);
		void parse_line(char *line, int len);

};

#endif
