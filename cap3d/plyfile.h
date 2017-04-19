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
	unsigned int count; //number of faces this vertex belong to
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

typedef struct tagVertex {
	float *v;
} Vertex;

typedef struct tagFace {
	unsigned char vertex_count;
	int vertex_indices[5];
	float normal[3];
	vector<Vertex> vertices;
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
	private:
		unsigned int num_of_real_vertex;
	public:
		vector<PlyProperty> properties;
		vector<CoVertex> coVertices;

		vector <Vertex> vertices;
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

			for(i=0; i<size; i++)
				delete[] vertices[i].v;

			for(i=0; i<fsize; i++) {
				for(j=0; j<faces[i].vertices.size(); j++)
					delete[] faces[i].vertices[j].v;
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
		int add_normal_vectors();

		void getBBox(BBox3d &bbox);
		int parse_line2(string line, vector<Token> &v);

		int generateCoVertices();
		/*
			Params:
				n [OUT] number of float returned
		*/
		float* getVertexBuffer(unsigned int &);

		float* getNormalBuffer(unsigned int &);

		/*
		 	For testing
		*/
		void print(ostream &out) {
			int i, j, k;
			int vertex_count = vertices.size();
			int face_count = faces.size();

			out << "Number of vertex: " << vertex_count << endl;
			out << "Number of faces: " << face_count << endl;
			out << "Number of real vertex: " << num_of_real_vertex << endl;
			out << "Number of float per vertex: " << float_stride << endl;
			out << "Vertices: " << endl;
			for(i=0; i<vertex_count; i++) {
				for(j=0; j<float_stride; j++)
					out << vertices[i].v[j] << " ";
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
