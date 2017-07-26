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
		} else if(tk == "imX") {
			Token t(CODE_PCD_IMX);
			v.push_back(t);
		} else if(tk == "imY") {
			Token t(CODE_PCD_IMY);
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
		} else if(tk == "comment" || tk == "converted" || tk == "from" || tk == "OBJ" ) {

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

	fields.clear();

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
			float_stride = fields.size();
			for(int i=0; i<vertex_count; i++) {
				std::getline(f, line);
				istringstream str(line);
				Vertex vt;
				vt.v = new float[float_stride];
				vt.count = float_stride;
				for(int j=0; j<float_stride; j++) {
					if(fields[j].code == pcd::RGB) {
						switch(fields[j].type) {
							case pcd::INT8:
							break;
							case pcd::UINT8:
							break;
							case pcd::INT16:
							break;
							case pcd::UINT16:
							break;
							case pcd::INT32:
							break;
							case pcd::UINT32:
							break;
							case pcd::FLOAT32:
								uint32_t color_value;
								str >> tmp;
								memcpy(&color_value, reinterpret_cast<const char*>(&tmp), sizeof(float));
								// cout << "[DEBUG] uint32_t Color: " << color_value << endl;
							break;
							case pcd::FLOAT64:
							break;
						}
					} else {
						str >> tmp;
						vt.v[j] = tmp / scale;
					}
				}
				vertices.push_back(vt);
			}
		} else if(tokens[0].code == CODE_PCD_FIELDS) {
			pcd::PointField f;
			for(int i=1; i<tokens.size(); i++) {
				switch(tokens[i].code) {
					case CODE_PCD_X:
						f.code = pcd::X;
						f.index = i-1;
						fields.push_back(f);
					break;

					case CODE_PCD_Y:
						f.code = pcd::Y;
						f.index = i-1;
						fields.push_back(f);
					break;

					case CODE_PCD_Z:
						f.code = pcd::Z;
						f.index = i-1;
						fields.push_back(f);
					break;

					case CODE_PCD_NORM_X:
						f.code = pcd::NORMAL_X;
						f.index = i-1;
						fields.push_back(f);
					break;

					case CODE_PCD_NORM_Y:
						f.code = pcd::NORMAL_Y;
						f.index = i-1;
						fields.push_back(f);
					break;

					case CODE_PCD_NORM_Z:
						f.code = pcd::NORMAL_Z;
						f.index = i-1;
						fields.push_back(f);
					break;

					case CODE_PCD_RGB:
						f.code = pcd::RGB;
						f.index = i-1;
						fields.push_back(f);
					break;

					case CODE_PCD_IMX:
						f.code = pcd::IMX;
						f.index = i-1;
						fields.push_back(f);
					break;

					case CODE_PCD_IMY:
						f.code = pcd::IMY;
						f.index = i-1;
						fields.push_back(f);
					break;
				}
			}
		} else if(tokens[0].code == CODE_PCD_TYPE) {
			for(int i=1; i<tokens.size(); i++) {
				switch(tokens[i].code) {
					case CODE_PCD_FLOAT:
						fields[i].type = pcd::FLOAT32;
					break;

					case CODE_PCD_SIGNED:
						fields[i].type = pcd::INT32;
					break;

					case CODE_PCD_UNSIGNED:
						fields[i].type = pcd::UINT32;
					break;
				}
			}
		} else if(tokens[0].code == CODE_PCD_POINTS) {
			vertex_count = (int)(tokens[1].value);
		}
	}

	cout << "[DEBUG] Number of vertex: " << vertex_count << endl;
	cout << "[DEBUG] Number of field per vertex: " << fields.size() << endl;

	f.close();
	result = new PCDModel3D(vertices, float_stride);
	return result;
}

