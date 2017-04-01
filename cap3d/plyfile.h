#ifndef __PLYFILE_H_
#define __PLYFILE_H_

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <gm.hpp>

using namespace std;
using namespace gm;

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

};

#endif
