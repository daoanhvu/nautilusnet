#include "../utils.h"
#include <cstring>
#include "pcdreader.h"

int PCDReader::parse_line2(string line, vector<Token> &v) {
	int error = 0;
	istringstream str(line);
	string tk;
	float val;

	//cout << line << endl;

	while(!str.eof()) {
		str >> tk;

		if(tk == "VERSION") {
			Token t(CODE_PCD_VERSION);
			v.push_back(t);
		} else if(tk == "FIELDS") {
			Token t(CODE_PCD_FIELDS);
			v.push_back(t);
		} else if(tk == "SIZE") {
			Token t(CODE_PCD_SIZE);
			v.push_back(t);
		}	else if(tk == "TYPE") {
			Token t(CODE_PCD_TYPE);
			v.push_back(t);
		} else if(tk == "COUNT") {
			Token t(CODE_PCD_COUNT);
			v.push_back(t);
		} else if(tk == "WIDTH") {
			Token t(CODE_PCD_WIDTH);
			v.push_back(t);
		} else if(tk == "HEIGHT") {
			Token t(CODE_PCD_HEIGHT);
			v.push_back(t);
		} else if(tk == "VIEWPOINT") {
			Token t(CODE_PCD_VIEWPOINT);
			v.push_back(t);
		} else if(tk == "POINTS") {
			Token t(CODE_PCD_POINTS);
			v.push_back(t);
		} else if(tk == "DATA") {
			Token t(CODE_PCD_DATA);
			v.push_back(t);
		} else if( isNumber(tk, val) ) {
			//cout << line << " NUmber: " << val << endl;
			Token t(CODE_NUMBER, val);
			v.push_back(t);
		} else if(tk == "rgb") {
			Token t(CODE_PCD_RGB);
			v.push_back(t);
		} else if(tk == "x") {
			Token t(CODE_PCD_X);
			v.push_back(t);
		} else if(tk == "y") {
			Token t(CODE_PCD_Y);
			v.push_back(t);
		} else if(tk == "z") {
			Token t(CODE_PCD_Z);
			v.push_back(t);
		} else if(tk == "normal_x") {
			Token t(CODE_PCD_NORM_X);
			v.push_back(t);
		} else if(tk == "normal_y") {
			Token t(CODE_PCD_NORM_Y);
			v.push_back(t);
		} else if(tk == "normal_z") {
			Token t(CODE_PCD_NORM_Z);
			v.push_back(t);
		} else if(tk == "ascii") {
			Token t(CODE_PCD_ASCII);
			v.push_back(t);
		} else if(tk == "F") {
			Token t(CODE_PCD_FLOAT);
			v.push_back(t);
		} else if(tk == "U") {
			Token t(CODE_PCD_UNSIGNED);
			v.push_back(t);
		} else if(tk == "I") {
			Token t(CODE_PCD_SIGNED);
			v.push_back(t);
		}
		else if(tk == "comment" || tk == "converted" || tk == "from" || tk == "OBJ" ) {

		}
	}

	return error;
}

Model3D* PCDReader::load(const char *filename, float scale) {
	ifstream f(filename);
	string line;
	vector<Token> tokens(0);
	bool read_vertex;
	int size;
	float tmp;
	int vertex_index, vertex_count;
	Model3D *result;
	if(f.fail()) {
		cout << "Failed to open this file " << filename << endl;
		return NULL;
	}

	int float_stride = 0;
	vector<Vertex> vertices;

	while(!f.eof()) {
		std::getline(f, line);
		tokens.clear();
		parse_line2(line, tokens);
		size = tokens.size();
		if(size == 2 && tokens[0].code == CODE_PCD_DATA) {

 			if(tokens[1].code == CODE_PCD_ASCII) {
 				//TODO: NOT Elemented yet!!!!
 			}

			//start read vertices
			read_vertex = true;

			vertices.reserve(vertex_count);

			for(int i=0; i<vertex_count; i++) {
				std::getline(f, line);
				istringstream str(line);
				Vertex vt;
				vt.v = new float[float_stride];
				vt.count = float_stride;
				for(int j=0; j<float_stride; j++) {
					str >> tmp;
					vt.v[j] = tmp / scale;
				}
				vertices.push_back(vt);
			}
		} else if(tokens[0].code == CODE_PCD_FIELDS) {
			int l = tokens.size();
			int idx = 1;
			while(idx < l) {
				if(tokens[idx].code == CODE_PCD_X || (tokens[idx].code == CODE_PCD_Y) || 
					tokens[idx].code == CODE_PCD_Z || tokens[idx].code == CODE_PCD_NORM_X || 
					tokens[idx].code == CODE_PCD_NORM_Y || tokens[idx].code == CODE_PCD_NORM_Z) {
					float_stride++;
				}
				idx++;
			}
		} else if(tokens[0].code == CODE_PCD_POINTS) {
			vertex_count = (int)(tokens[1].value);
		}
	}

	f.close();
	result = new PCDModel3D(vertices, float_stride);
	return result;
}

