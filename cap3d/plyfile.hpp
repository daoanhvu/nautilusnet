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

class PlyFile {
	private:
		vector<PlyProperty> properties;
		vector<vec3> vertices;
		vector<Face> faces;

	public:
		PlyFile();
		PlyFile(const char *filename);
		PlyFile(string filename);


		int load();

};

PlyFile::PlyFile() {

}

PlyFile::PlyFile(const char *filename) {
	ifstream f(filename);
	char line[128];
	int i;
	float x, y, z;

	try {
		
		if(f.fail()) {
			throw new OpenFileException;
		}


	} catch(const OpenFileException &e) {
		std::cout << e.what();
	}
}

PlyFile::PlyFile(string filename) {

}

/*
	In case that user used default constructor, then she need to call
	this method to load a model
*/
int PlyFile::load(const char* filename) {

	return 0;
}
