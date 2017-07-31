#include "../utils.h"
#include <cstring>
#include "pcdreader.h"

#ifdef __linux__
	#include <cstdint>
#endif

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
		} else if(tk == "binary") {
			Token t(CODE_PCD_BINARY);
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
	ifstream file(filename);
	string line;
	vector<Token> tokens(0);
	bool read_vertex;
	int size;
	float tmp;
	int vertex_index, vertex_count;
	int vertex_data_type = 0;
	Model3D *result;
	if(file.fail()) {
		cout << "Failed to open this file " << filename << endl;
		return NULL;
	}

	int float_stride = 0;
	int field_size;
	int offset;
	int color = 0;
	int colorIndex = -1;
	vector<Vertex> vertices;
	fields.clear();

	result = new PCDModel3D();

	while(!file.eof()) {
		std::getline(file, line);
		tokens.clear();
		parse_line2(line, tokens);
		size = tokens.size();
		if(size == 2 && tokens[0].code == CODE_PCD_DATA) {

 			if(tokens[1].code == CODE_PCD_ASCII) {
 				//TODO: NOT Elemented yet!!!!
 			}

 			//If there is color field in the model and,
 			//if color mode is RGB we add 3 - 1 = 2
 			//if color mode is RGBA we add 4 - 1 = 2
 			if(color > 0) {
 				float_stride = fields.size() + color - 1; 
 			}

			//start read vertices
			read_vertex = true;
			uint8_t r, g, b;

			vertices.reserve(vertex_count);
			field_size = fields.size();
			for(int i=0; i<vertex_count; i++) {

				if(vertex_data_type == 0) {
					//ASCII format 
					std::getline(file, line);
					istringstream str(line);
					Vertex vt;
					vt.v = new float[float_stride];
					// vt.face_size = 0;
					offset = 0;
					for(int j=0; j<field_size; j++) {
						if(fields[j].code == RGB) {
							switch(fields[j].type) {
								case type::INT8:
								break;
								case type::UINT8:
								break;
								case type::INT16:
								break;
								case type::UINT16:
								break;
								case type::INT32:
								break;
								case type::UINT32:
								break;
								case type::FLOAT32:
									uint32_t color_value;
									str >> tmp;
									memcpy(&color_value, reinterpret_cast<const char*>(&tmp), sizeof(float));
									// cout << "[DEBUG] uint32_t Color: " << color_value << endl;
									r = (color_value >> 16) & 0x0000ff;
									g = (color_value >> 8) & 0x0000ff;
									b = color_value & 0x0000ff;
									vt.v[offset++] = r/255.0f;
									vt.v[offset++] = g/255.0f;
									vt.v[offset++] = b/255.0f;
								break;
								case type::FLOAT64:
								break;
							}
						} else if(fields[j].code == RGBA) {

						} else {
							str >> tmp;
							vt.v[offset++] = tmp;
						}
					}
				} else {
					//Format = BINARY
					
				}
				
				// cout << "[DEBUG] vertex " << i << " " << vt.v[0] << ", " << vt.v[1] << ", " << vt.v[2] << ", " << vt.v[3] << ", " << vt.v[4] << ", " << vt.v[5] << ", " << vt.v[6] << ", " << vt.v[7] << endl;
				vertices.push_back(vt);
			}
		} else if(tokens[0].code == CODE_PCD_FIELDS) {
			PointField f;
			offset = 0;
			VertexAttrib att;
			for(int i=1; i<tokens.size(); i++) {
				switch(tokens[i].code) {
					case CODE_PCD_X:
						cout << "[DEBUG] X located at " << fields.size() << endl;
						f.code = X;
						f.index = i-1;
						fields.push_back(f);

						att.code = POSITION;
						att.offset = offset;
						result->addAttrib(att);
						offset += 3;
					break;

					case CODE_PCD_Y:
						cout << "[DEBUG] Y located at " << fields.size() << endl;
						f.code = Y;
						f.index = i-1;
						fields.push_back(f);
					break;

					case CODE_PCD_Z:
						cout << "[DEBUG] Y located at " << fields.size() << endl;
						f.code = Z;
						f.index = i-1;
						fields.push_back(f);
					break;

					case CODE_PCD_NORM_X:
						f.code = NORMAL_X;
						f.index = i-1;
						fields.push_back(f);

						att.code = NORMAL;
						att.offset = offset;
						result->addAttrib(att);
						offset += 3;
					break;

					case CODE_PCD_NORM_Y:
						f.code = NORMAL_Y;
						f.index = i-1;
						fields.push_back(f);
					break;

					case CODE_PCD_NORM_Z:
						f.code = NORMAL_Z;
						f.index = i-1;
						fields.push_back(f);
					break;

					case CODE_PCD_RGB:
						cout << "[DEBUG] grb located at " << fields.size() << endl;
						f.code = RGB;
						f.index = i-1;
						fields.push_back(f);
						color = 3;
						colorIndex = f.index;

						att.code = COLOR3;
						att.offset = offset;
						result->addAttrib(att);
						offset += 3;
					break;

					case CODE_PCD_RGBA:
						f.code = RGBA;
						f.index = i-1;
						fields.push_back(f);
						color = 4;
						colorIndex = f.index;

						att.code = COLOR4;
						att.offset = offset;
						result->addAttrib(att);
						offset += 4;
					break;

					case CODE_PCD_IMX:
						cout << "[DEBUG] imX located at " << fields.size() << endl;
						f.code = IMX;
						f.index = i-1;
						fields.push_back(f);

						att.code = TEXTURE;
						att.offset = offset;
						result->addAttrib(att);
						offset += 2;
					break;

					case CODE_PCD_IMY:
						cout << "[DEBUG] imY located at " << fields.size() << endl;
						f.code = IMY;
						f.index = i-1;
						fields.push_back(f);
					break;
				}
			}
		} else if(tokens[0].code == CODE_PCD_TYPE) {
			for(int i=1; i<tokens.size(); i++) {
				switch(tokens[i].code) {
					case CODE_PCD_FLOAT:
						fields[i].type = type::FLOAT32;
					break;

					case CODE_PCD_SIGNED:
						fields[i].type = type::INT32;
					break;

					case CODE_PCD_UNSIGNED:
						fields[i].type = type::UINT32;
					break;
				}
			}
		} else if(tokens[0].code == CODE_PCD_POINTS) {
			vertex_count = (int)(tokens[1].value);
		} else if(tokens[0].code == CODE_PCD_DATA) {
			if(tokens[1].code == CODE_PCD_ASCII) {
				vertex_data_type = 0;
			} else if(tokens[1].code == CODE_PCD_BINARY) {
				vertex_data_type = 1;
			}
		}
	}

	result->setAll(vertices, float_stride);
	result->scaleToFit(scale);

	cout << "[DEBUG] Number of vertex: " << vertex_count << endl;
	cout << "[DEBUG] Number of field per vertex: " << fields.size() << endl;
	// (dynamic_cast<PCDModel3D*>(result))->print(cout);

	file.close();
	return result;
}

