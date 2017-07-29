
#include "plyreader.h"
#include "utils.h"
#include <cstring>

type::FieldType getFieldType(int type) {
	type::FieldType t;
	switch(type) {
		case CODE_UNIT8:
		case CODE_UCHAR:
			t = type::UINT8;
		break;

		// case CODE_UNIT16:
		// 	t = type::UINT16;
		// break;

		case CODE_INT32:
			t = type::INT32;
		break;

		case CODE_FLOAT32:
			t = type::FLOAT32;
		break;

		default:
			t = type::FLOAT32;
		break;
	}

	return t;
}

int PLYReader::parse_line2(string line, vector<Token> &v) {
	int error = 0;
	istringstream str(line);
	string tk;
	float val;

	//cout << line << endl;

	while(!str.eof()) {
		str >> tk;

		if(tk == "ply") {
			Token t(CODE_PLY);
			v.push_back(t);
		} else if(tk == "property") {
			Token t(CODE_PROPERTY);
			v.push_back(t);
		} else if(tk == "uint8") {
			Token t(CODE_UNIT8);
			v.push_back(t);
		}	else if(tk == "uint32") {
			Token t(CODE_UNIT32);
			v.push_back(t);
		} else if(tk == "float32") {
			Token t(CODE_FLOAT32);
			v.push_back(t);
		} else if(tk == "uchar") {
			Token t(CODE_UCHAR);
			v.push_back(t);
		} else if(tk == "list") {
			Token t(CODE_LIST);
			v.push_back(t);
		} else if(tk == "vertex") {
			Token t(CODE_VERTEX);
			v.push_back(t);
		} else if(tk == "face") {
			Token t(CODE_FACE);
			v.push_back(t);
		} else if(tk == "element") {
			Token t(CODE_ELEMENT);
			v.push_back(t);
		} else if(tk == "end_header") {
			Token t(CODE_END_HEADER);
			v.push_back(t);
		} else if( isNumber(tk, val) ) {
			//cout << line << " NUmber: " << val << endl;
			Token t(CODE_NUMBER, val);
			v.push_back(t);
		} else if(tk == "vertex_indices") {
			Token t(CODE_VERTEX_INDICES);
			v.push_back(t);
		} else if(tk == "x") {
			Token t(CODE_COORD_X);
			v.push_back(t);
		} else if(tk == "y") {
			Token t(CODE_COORD_Y);
			v.push_back(t);
		} else if(tk == "z") {
			Token t(CODE_COORD_Z);
			v.push_back(t);
		} else if(tk == "red") {
			Token t(CODE_RED);
			v.push_back(t);
		} else if(tk == "green") {
			Token t(CODE_GREEN);
			v.push_back(t);
		} else if(tk == "blue") {
			Token t(CODE_BLUE);
			v.push_back(t);
		} 
		else if(tk == "comment" || tk == "converted" || tk == "from" || tk == "OBJ" ) {

		}
	}

	return error;
}

//TODO: Not implemented yet
int PLYReader::load(const char *filename, float scale, Model3D &model) {
	return -1;
}

Model3D* PLYReader::load(const char *filename, float scale) {
	ifstream file(filename);
	string line;
	vector<Token> tokens(0);
	bool read_vertex;
	int size, v_per_face;
	bool got_color = false;
	bool start_reading_vertex_properties = false;
	bool start_reading_camera_properties = false;
	bool start_reading_face_properties = false;
	int offset = 0;
	float tmp;
	int vertex_index, vertex_count, face_count;

	if(file.fail()) {
		cout << "Failed to open this file " << filename << endl;
		return NULL;
	}

	PLYModel3D *result = new PLYModel3D();

	int *face_indices;
	int float_stride = 0;
	vector<Vertex> vertices;
	vector<Face> faces;
	type::FieldType type;

	while(!file.eof()) {
		std::getline(file, line);
		tokens.clear();
		parse_line2(line, tokens);
		size = tokens.size();
		if(size == 1 && tokens[0].code == CODE_END_HEADER) {
			//start read vertices
			read_vertex = true;

			vertices.reserve(vertex_count);
			// cout << "[DEBUG] vertex count: " << vertex_count << endl;
			for(int i=0; i<vertex_count; i++) {
				std::getline(file, line);
				istringstream str(line);
				Vertex vt;
				vt.v = new float[float_stride];
				vt.face_size = 0;
				for(int j=0; j<float_stride; j++) {
					if(fields[j].code == RED || fields[j].code == GREEN || 
							fields[j].code == BLUE || fields[j].code == ALPHA) {
						switch(fields[j].type) {
							case type::UINT8:
								str >> tmp;
								vt.v[j] = tmp/255.0f;
							break;
							case type::FLOAT32:
								str >> tmp;
								vt.v[j] = tmp;
							break;

							default:
							break;
						}
					} else {
						str >> tmp;
						vt.v[j] = tmp;
					}
				}
				vertices.push_back(vt);
			}
			// cout << "Number of face: " << face_count << endl;
			faces.reserve(face_count);
			//Now read face
			for(int i=0; i<face_count; i++) {
				std::getline(file, line);
				istringstream str1(line);
				Face face;
				str1 >> v_per_face;
				face.vertex_count = v_per_face;
				// cout << "Face line("<< i <<"): " << line << " v_per_face: "<< v_per_face << endl;
				for(int j=0; j<v_per_face; j++) {
					str1 >> vertex_index;
					face.vertex_indices[j] = vertex_index;

					if(vertices[vertex_index].face_size >= vertices[vertex_index].log_face_size) {
						vertices[vertex_index].log_face_size += 8;
						face_indices = new int[vertices[vertex_index].log_face_size];
						std::memcpy(face_indices, vertices[vertex_index].face_indices, sizeof(int) * vertices[vertex_index].face_size);
						delete[] vertices[vertex_index].face_indices;
						vertices[vertex_index].face_indices = face_indices;
					}
					vertices[vertex_index].face_indices[vertices[vertex_index].face_size++] = i;
				}
				faces.push_back(face);
			}
		} else if(tokens[0].code == CODE_PROPERTY) {
			PointField f;
			VertexAttrib att;

			f.type = getFieldType(tokens[1].code);
			f.index = fields.size();
			switch(tokens[2].code) {
				case CODE_COORD_X:
					f.code = X;		
					fields.push_back(f);

					att.code = POSITION;
					att.offset = offset;
					// cout << "[DEBUG] Position offset: " << offset;
					result->addAttrib(att);
					offset += 3;
					float_stride++;
				break;

				case CODE_COORD_Y:
					f.code = Y;
					fields.push_back(f);

					float_stride++;
				break;

				case CODE_COORD_Z:
					f.code = Z;
					fields.push_back(f);

					float_stride++;
				break;

				case CODE_RED:
					f.code = RED;
					fields.push_back(f);

					if(!got_color) {
						att.code = COLOR3;
						att.offset = offset;
						result->addAttrib(att);
						offset += 3;
						got_color = true;
					}
					float_stride++;
				break;

				case CODE_GREEN:
					f.code = GREEN;
					fields.push_back(f);

					if(!got_color) {
						att.code = COLOR3;
						att.offset = offset;
						result->addAttrib(att);
						offset += 3;
						got_color = true;
					}
					float_stride++;
				break;

				case CODE_BLUE:
					f.code = GREEN;
					fields.push_back(f);

					if(!got_color) {
						att.code = COLOR3;
						att.offset = offset;
						result->addAttrib(att);
						offset += 3;
						got_color = true;
					}
					float_stride++;
				break;

			}
		} else if(tokens[0].code == CODE_ELEMENT) {
			if(tokens[1].code == CODE_VERTEX) {
				vertex_count = (int)(tokens[2].value);
				start_reading_vertex_properties = true;
				start_reading_face_properties = false;
				start_reading_camera_properties = false;
			} else if(tokens[1].code == CODE_FACE) {
				face_count = (int)(tokens[2].value);
				start_reading_face_properties = true;
				start_reading_vertex_properties = false;
				start_reading_camera_properties = false;
			} else if(tokens[1].code == CODE_CAMERA) {
				start_reading_face_properties = false;
				start_reading_vertex_properties = false;
				start_reading_camera_properties = true;
			}
		}
	}

	file.close();
	result->setAll(vertices, faces, float_stride);

	//Need safe type-casting???? NO
	result->scaleToFit(scale);
	((PLYModel3D*)result)->add_normal_vectors();

	if(result->getColorOffset() == -1) {
		//Add default color
		((PLYModel3D*)result)->addDefaultColor(0.3f, 0.5f, 0.6f);
	}

	//DEBUG
	result->print(cout);

	return result;
}

//TODO: Not implemented yet
void PLYReader::load(const char *filename, Model3D &model) {

}

Model3D* PLYReader::load(const char *filename) {
	ifstream f(filename);
	string line;
	vector<Token> tokens(0);
	bool read_vertex;
	bool start_reading_vertex_properties = false;
	bool start_reading_camera_properties = false;
	bool start_reading_face_properties = false;
	bool got_color = false;
	int offset = 0;
	int size, v_per_face;
	float tmp;
	int vertex_index, vertex_count, face_count;
	
	if(f.fail()) {
		cout << "Failed to open this file " << filename << endl;
		return NULL;
	}

	PLYModel3D *result = new PLYModel3D();

	int *face_indices;
	int float_stride = 0;
	vector<Vertex> vertices;
	vector<Face> faces;

	while(!f.eof()) {
		std::getline(f, line);
		tokens.clear();
		parse_line2(line, tokens);
		size = tokens.size();
		if(size == 1 && tokens[0].code == CODE_END_HEADER) {
			//start read vertices
			read_vertex = true;

			vertices.reserve(vertex_count);

			for(int i=0; i<vertex_count; i++) {
				std::getline(f, line);
				istringstream str(line);
				Vertex vt;
				vt.v = new float[float_stride];
				vt.face_size = 0;
				for(int j=0; j<float_stride; j++) {
					str >> tmp;
					vt.v[j] = tmp;
				}
				vertices.push_back(vt);
			}
			// cout << "Number of face: " << face_count << endl;
			faces.reserve(face_count);
			//Now read face
			for(int i=0; i<face_count; i++) {
				std::getline(f, line);
				istringstream str1(line);
				Face face;
				str1 >> v_per_face;
				face.vertex_count = v_per_face;
				// cout << "Face line("<< i <<"): " << line << " v_per_face: "<< v_per_face << endl;
				for(int j=0; j<v_per_face; j++) {
					str1 >> vertex_index;
					face.vertex_indices[j] = vertex_index;

					if(vertices[vertex_index].face_size >= vertices[vertex_index].log_face_size) {
						vertices[vertex_index].log_face_size += 8;
						face_indices = new int[vertices[vertex_index].log_face_size];
						std::memcpy(face_indices, vertices[vertex_index].face_indices, sizeof(int) * vertices[vertex_index].face_size);
						delete[] vertices[vertex_index].face_indices;
						vertices[vertex_index].face_indices = face_indices;
					}
					vertices[vertex_index].face_indices[vertices[vertex_index].face_size++] = i;
				}
				faces.push_back(face);
			}
		} else if(tokens[0].code == CODE_PROPERTY) {
			VertexAttrib att;
			switch(tokens[2].code) {
				case CODE_COORD_X:
					att.code = POSITION;
					att.offset = offset;
					result->addAttrib(att);
					offset += 3;
					float_stride++;
				break;

				case CODE_COORD_Y:
					float_stride++;
				break;

				case CODE_COORD_Z:
					float_stride++;
				break;

				case CODE_RED:
					if(!got_color) {
						att.code = COLOR3;
						att.offset = offset;
						result->addAttrib(att);
						offset += 3;
						got_color = true;
					}
					float_stride++;
				break;

				case CODE_GREEN:
					if(!got_color) {
						att.code = COLOR3;
						att.offset = offset;
						result->addAttrib(att);
						offset += 3;
						got_color = true;
					}
					float_stride++;
				break;

				case CODE_BLUE:
					if(!got_color) {
						att.code = COLOR3;
						att.offset = offset;
						result->addAttrib(att);
						offset += 3;
						got_color = true;
					}
					float_stride++;
				break;

			}
		} else if(tokens[0].code == CODE_ELEMENT) {
			if(tokens[1].code == CODE_VERTEX) {
				vertex_count = (int)(tokens[2].value);
				start_reading_vertex_properties = true;
				start_reading_face_properties = false;
				start_reading_camera_properties = false;
			} else if(tokens[1].code == CODE_FACE) {
				face_count = (int)(tokens[2].value);
				start_reading_face_properties = true;
				start_reading_vertex_properties = false;
				start_reading_camera_properties = false;
			} else if(tokens[1].code == CODE_CAMERA) {
				start_reading_face_properties = false;
				start_reading_vertex_properties = false;
				start_reading_camera_properties = true;
			}
		}
	}

	f.close();

	result->setAll(vertices, faces, float_stride);

	return result;
}

int PLYReader::save(const Model3D *model, const char *filename) {
	return 0;
}
