#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <gm.hpp>

using namespace std;

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
	private:
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

PlyFile::PlyFile() {
	vertices = 0; //NULL
	faces = 0; //NULL
	vertex_count = 0;
	face_count = 0;
}

PlyFile::PlyFile(const char *filename) {
	ifstream f(filename);
	char line[128];
	int i;
	float x, y, z;

	vertices = 0; //NULL
	faces = 0; //NULL
	vertex_count = 0;
	face_count = 0;

	try {

		if(f.fail()) {
			throw new OpenFileException;
		}


	} catch(const OpenFileException &e) {
		std::cout << e.what();
	}
}

PlyFile::PlyFile(string filename) {
	vertices = 0;
	vertex_count = 0;
	face_count = 0;
}

/*
	In case that user used default constructor, then she need to call
	this method to load a model
*/
int PlyFile::load(const char* filename) {
	ifstream f(filename);
	char line[128];
	int i;
	float x, y, z;

	if(f.fail()) {
		cout << "Failed to open this file " << filename << endl;
		return 1;
	}
	BBox3d bbox;
	bbox.minx = 999999.0f;
	bbox.maxx = -9999.0f;
	bbox.miny = 999999.0f;
	bbox.maxy = -9999.0f;
	bbox.minz = 999999.0f;
	bbox.maxz = -9999.0f;

	while(!f.eof()) {
		f.getline(line, 128);
		istringstream ls(line);
		string token;
		ls >> token;

		if(token == "end_header") {

			this->vertices = new float[vertex_count * properties.size()];
			int offs;
			for(i=0; i<this->vertex_count; i++) {
				offs = i * properties.size();
				f.getline(line, 64);
				istringstream ls1(line);
				ls1 >> x;
				ls1 >> y;
				ls1 >> z;

				if(x < bbox.minx)
					bbox.minx = x;
				if(x > bbox.maxx)
					bbox.maxx = x;

				if(y < bbox.miny)
					bbox.miny = y;
				if(y > bbox.maxy)
					bbox.maxy = y;

				if(z < bbox.minz)
					bbox.minz = z;
				if(z > bbox.maxz)
					bbox.maxz = z;

				this->vertices[offs] = x/150.0f;
				this->vertices[offs + 1] = y/150.0f;
				this->vertices[offs + 2] = z/150.0f;
			}

			//read faces
			faces = new Face[face_count];
			for(i=0; i<face_count; i++) {
				f.getline(line, 64);
				istringstream ls2(line);

				//read the number of vertex of this face
				ls2 >> faces[i].vertex_count;
				for(int j=0; j<faces[i].vertex_count; j++) {
					ls2 >> faces[i].vertex_indices[j];
				}
			}
			break;
		} else if(token == "element") {
			string tk2;
			ls >> tk2;
			if(tk2 == "vertex"){
				ls >> vertex_count;
				cout << "Number of vertex " << this->vertex_count << std::endl;
			} else if(tk2 == "face") {
				ls >> face_count;
				cout << "Number of faces: " << face_count << std::endl;
			}
		} else if(token == "property") {
			string tk3;
			ls >> tk3;
			cout << "Property: " << tk3 << endl;
		}
		//cout << line << std::endl;
	}

	f.close();
	return 0;
}
