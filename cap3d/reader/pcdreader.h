#ifndef _PCDREADER_H_
#define _PCDREADER_H_

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include "reader.h"
#include "pcdmodel3d.h"

class PCDReader: public Reader {
private:
protected:
		void readpoints(std::ifstream& file, unsigned int offs, int format_type, int vertex_count, vector<Vertex>& vertices, int float_stride);
public:
	PCDReader() {
	}

	virtual ~PCDReader(){};
	
	int parse_line2(string line, vector<Token> &v);
	virtual Model3D* load(const char *filename, float scale);
};

#endif