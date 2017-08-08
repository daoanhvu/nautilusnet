
#include "plyreader.h"
#include "utils.h"
#include <cstring>

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
		} else if(tk == "format") {
			Token t(CODE_FORMAT);
			v.push_back(t);
		} else if(tk == "ascii") {
			Token t(CODE_ASCII);
			v.push_back(t);
		} else if(tk == "binary") {
			Token t(CODE_BINARY);
			v.push_back(t);
		} else if(tk == "binary_little_endian") {
			Token t(CODE_BINARY_LITTLE_ENDIAN);
			v.push_back(t);
		} else if(tk == "binary_big_endian") {
			isBigEndian = true;
			Token t(CODE_BINARY_BIG_ENDIAN);
			v.push_back(t);
		} else if(tk == "property") {
			Token t(CODE_PROPERTY);
			v.push_back(t);
		} else if(tk == "uint8") {
			Token t(CODE_UINT8);
			v.push_back(t);
		} else if(tk == "uint16") {
			Token t(CODE_UINT16);
			v.push_back(t);
		} else if(tk == "uint32") {
			Token t(CODE_UINT32);
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

/**
	property <data_type> <component>
	Note: <component> is one of X, Y, Z, RED, GREEN, BLUE, NORMAL_X, NORMAL_Y, NORMAL_Z
*/
void PLYReader::readProperty(const vector<Token>& tokens, int element_type, PLYModel3D *model, int &float_stride) {
	PointField pf;

	if(element_type == CODE_VERTEX) {
		/**
			TODO: We use OpenGL as the main graphics library of this project, so I cast every components to 
			floats in order to put them into the OpenGL pipeline
		*/
		float_stride++;

		pf.code = getCodeByValue(tokens[2].code);
		cout << "CODE: " << tokens[1].code << endl;
		int size = readPointField(pf, tokens[1].code);
		pf.size = size;
		fields.push_back(pf);
	} else if(element_type == CODE_FACE) {
		if(tokens[1].code == CODE_LIST) {
			vertex_per_face_type = tokens[2].code;
			vertex_index_type = tokens[3].code;
		}
	} else if(element_type == CODE_CAMERA) {

	}
}

//TODO: Not implemented yet
int PLYReader::load(const char *filename, float scale, Model3D &model) {
	return -1;
}

Model3D* PLYReader::load(const char *filename, float scale, bool should_add_normal, bool should_add_color) {
	ifstream file(filename, std::ios::binary);
	string line;
	vector<Token> tokens(0);
	bool read_header = true;
	int element_type;
	int format_type;
	int vertex_count, face_count;

	if(file.fail()) {
		cout << "Failed to open this file " << filename << endl;
		return NULL;
	}

	PLYModel3D *result = new PLYModel3D();

	int float_stride = 0;
	vector<Vertex> vertices;
	vector<Face> faces;

	while(read_header) {
		std::getline(file, line);
		tokens.clear();
		parse_line2(line, tokens);
		// size = tokens.size();
		if(tokens[0].code == CODE_END_HEADER) {
			read_header = false;
		} else if(tokens[0].code == CODE_PROPERTY) {
			readProperty(tokens, element_type, result, float_stride);
		} else if(tokens[0].code == CODE_FORMAT) {
			format_type = tokens[1].code;
		} else if(tokens[0].code == CODE_ELEMENT) {
			element_type = tokens[1].code;
			if(tokens[1].code == CODE_VERTEX) {
				vertex_count = (int)(tokens[2].value);
			} else if(tokens[1].code == CODE_FACE) {
				face_count = (int)(tokens[2].value);
			}
		}
	}
	cout << "[DEBUG] float_stride: " << float_stride << endl;
	for(int l=0; l<fields.size(); l++) {
		cout << "[DEBUG] Attribute " << l << endl;
		cout << "[DEBUG] Code: " << fields[l].code << endl;
		cout << "[DEBUG] Type: " << fields[l].type << endl;
		cout << "[DEBUG] Size: " << fields[l].size << endl;
	}

	fillModelAttributes(result);

	vertices.reserve(vertex_count);
	faces.reserve(face_count);
	readpoints(file, 0, format_type, vertex_count, vertices, faces, face_count, float_stride);


	file.close();
	result->setAll(vertices, faces, float_stride);

	//Need safe type-casting???? NO
	result->scaleToFit(scale);

	if(should_add_normal) {
		//((PLYModel3D*)result)->add_normal_vectors();
		dynamic_cast<PLYModel3D*>(result)->add_normal_vectors();	
	}
	

	if( (result->getColorOffset() == -1) && should_add_color) {
		//Add default color
		// ((PLYModel3D*)result)->addDefaultColor(0.3f, 0.5f, 0.6f);
		dynamic_cast<PLYModel3D*>(result)->addDefaultColor(0.3f, 0.5f, 0.6f);
	}

	//DEBUG
	// cout << "[DEBUG] GOT HERE!!! "<< endl;
	// result->print(cout);

	return result;
}

//TODO: Not implemented yet
void PLYReader::load(const char *filename, Model3D &model) {

}

Model3D* PLYReader::load(const char *filename) {
	ifstream file(filename, std::ios::binary);
	string line;
	vector<Token> tokens(0);
	bool read_header = true;
	int element_type;
	int format_type;
	int vertex_count, face_count;
	
	if(file.fail()) {
		cout << "Failed to open this file " << filename << endl;
		return NULL;
	}

	PLYModel3D *result = new PLYModel3D();

	int float_stride = 0;
	vector<Vertex> vertices;
	vector<Face> faces;

	while(read_header) {
		std::getline(file, line);
		tokens.clear();
		parse_line2(line, tokens);
		// size = tokens.size();
		if(tokens[0].code == CODE_END_HEADER) {
			//start read vertices
			read_header = false;
		} else if(tokens[0].code == CODE_PROPERTY) {
			readProperty(tokens, element_type, result, float_stride);
		} else if(tokens[0].code == CODE_FORMAT) {
			format_type = tokens[1].code;
		} else if(tokens[0].code == CODE_ELEMENT) {
			element_type = tokens[1].code;
			if(tokens[1].code == CODE_VERTEX) {
				vertex_count = (int)(tokens[2].value);
			} else if(tokens[1].code == CODE_FACE) {
				face_count = (int)(tokens[2].value);
			}
		}
	}

	cout << "[DEBUG] float_stride: " << float_stride << endl;

	fillModelAttributes(result);
	vertices.reserve(vertex_count);
	faces.reserve(face_count);
	readpoints(file, 0, format_type, vertex_count, vertices, faces, face_count, float_stride);

	file.close();

	result->setAll(vertices, faces, float_stride);

	return result;
}

int PLYReader::save(const Model3D *model1, const char *filename, int format) {
	ofstream file(filename, std::ios::binary);
	std::ostringstream oss;
	VertexAttrib att;


	const PLYModel3D *model = dynamic_cast<const PLYModel3D*>(model1);

	int face_count = model->getFaceCount();
	int vertex_count = model->getVertexCount();
	int attrib_count = model->getAttribCount();


	//Header
	oss << "ply";
		
	if(format == CODE_BINARY || format == CODE_BINARY_LITTLE_ENDIAN) {

		oss << "\nformat binary_little_endian 1.0";
		oss << "\nelement vertex " << vertex_count;

		for(int i=0; i<attrib_count; i++) {
			att = model->getAttrib(i);
			switch(att.code) {
				case POSITION:
					oss << "\nproperty float x";
					oss << "\nproperty float y";
					oss << "\nproperty float z";
				break;

				case COLOR3:
					oss << "\nproperty float red";
					oss << "\nproperty float green";
					oss << "\nproperty float blue";
				break;

				case COLOR4:
					oss << "\nproperty float red";
					oss << "\nproperty float green";
					oss << "\nproperty float blue";
					oss << "\nproperty float alpha";
				break;

				case NORMAL:
					oss << "\nproperty float nx";
					oss << "\nproperty float ny";
					oss << "\nproperty float nz";
				break;

				case TEXTURE:
					oss << "\nproperty float imx";
					oss << "\nproperty float imy";
				break;
			}

		}

		if(face_count>0) {
			oss << "\nelement face " << face_count;
			oss << "\nproperty list uchar int vertex_index";
		}

		// End header
  		oss << "\nend_header\n";
  		//flush header
		file << oss.str();
		int stride = model->getFloatStride();
		std::cout << "[DEBUG] float_stride in save(): " << stride << std::endl;
		char tmp[4];
		Vertex vt;

		for(int i=0; i<vertex_count; i++) {
			vt = model->getVertex(i);
			for(int k=0; k<stride; k++) {
				file.write(reinterpret_cast<const char *>(vt.v + k), sizeof(float));
			}
		}

		Face face;
		for(int i=0; i<face_count; i++) {
			face = model->getFace(i);
			file.write(reinterpret_cast<const char *>(&face.vertex_count), sizeof(unsigned char));
			for(int k=0; k<face.vertex_count; k++) {
				file.write(reinterpret_cast<const char *>(face.vertex_indices + k), sizeof(int));
			}
		}
		
	} else if(format == CODE_ASCII) {
		oss << "\nformat ascii 1.0";
		oss << "\nelement vertex " << vertex_count;

		for(int i=0; i<attrib_count; i++) {
			att = model->getAttrib(i);
			switch(att.code) {
				case POSITION:
					oss << "\nproperty float x";
					oss << "\nproperty float y";
					oss << "\nproperty float z";
				break;

				case COLOR3:
					oss << "\nproperty float red";
					oss << "\nproperty float green";
					oss << "\nproperty float blue";
				break;

				case COLOR4:
					oss << "\nproperty float red";
					oss << "\nproperty float green";
					oss << "\nproperty float blue";
					oss << "\nproperty float alpha";
				break;

				case NORMAL:
					oss << "\nproperty float nx";
					oss << "\nproperty float ny";
					oss << "\nproperty float nz";
				break;

				case TEXTURE:
					oss << "\nproperty float imx";
					oss << "\nproperty float imy";
				break;
			}

		}

		if(face_count>0) {
			oss << "\nelement face " << face_count;
			oss << "\nproperty list uchar int vertex_index";
		}

		// End header
  		oss << "\nend_header\n";
  		//flush header
		file << oss.str();
	}


	file.close();
	return 0;
}


void PLYReader::readpoints(std::ifstream& file, unsigned int offs,
				int format_type, int vertex_count, 
				vector<Vertex>& vertices, vector<Face>& faces, int face_count, int float_stride) {

	std::string line;

	float tmp_float;
	short temp_short;
	int temp_int;
	unsigned char temp_uchar;
	char temp_char;

	char buff[4];
	int v_per_face, vertex_index;
	int *face_indices;
	int field_size = fields.size();
	int offset;
	Vertex vt;
	Face face;

	if(format_type == CODE_ASCII) {
		//Reading vertices data
		for(int i=0; i<vertex_count; i++) {
			std::getline(file, line);
			istringstream str(line);
			vt.v = new float[float_stride];
			vt.face_size = 0;
			for(int j=0; j<field_size; j++) {
				if(fields[j].code == RED || fields[j].code == GREEN || 
						fields[j].code == BLUE || fields[j].code == ALPHA) {
					switch(fields[j].type) {
						case type::UINT8:
							str >> tmp_float;
							vt.v[j] = tmp_float/255.0f;
						break;

						case type::FLOAT32:
							str >> tmp_float;
							vt.v[j] = tmp_float;
						break;

						default:
						break;
					}
				} else {
					str >> tmp_float;
					vt.v[j] = tmp_float;
				}
			}
			vertices.push_back(vt);
		}
		//Reading face data
		for(int i=0; i<face_count; i++) {
			std::getline(file, line);
			istringstream str1(line);
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
	} else {
		int vertex_size = getVertexSize();
		char *vertex_src = new char[vertex_size];

		//Reading vertices data
		for(int i=0; i<vertex_count; i++) {
			//read all data about vertext i to vertex_src
			file.read(vertex_src, vertex_size);

			//
			vt.v = new float[float_stride];

			offset = 0;
			for(int j=0; j<field_size; j++) {
				if(fields[j].code == RED || fields[j].code == GREEN || 
							fields[j].code == BLUE || fields[j].code == ALPHA) {
					switch(fields[j].type) {	
						case type::INT8:
						case type::UINT8:
							type_cast<unsigned char>(&temp_uchar, vertex_src+offset, isBigEndian);
							vt.v[j] = temp_uchar/255.0f;
						break;

						case type::FLOAT32:
							type_cast_float<float>(&tmp_float, vertex_src+offset, isBigEndian);
							vt.v[j] = tmp_float;
						break;

						default:
						break;
					}
				} else {
					switch(fields[j].type) {	
						case type::INT8:
						case type::UINT8:
							type_cast<unsigned char>(&temp_uchar, vertex_src+offset, isBigEndian);
							vt.v[j] = static_cast<float>(temp_uchar);
						break;

						case type::INT16:
						case type::UINT16:
							type_cast<short>(&temp_short, vertex_src+offset, isBigEndian);
							vt.v[j] = static_cast<float>(temp_short);
						break;

						case type::INT32:
						case type::UINT32:
							type_cast<int>(&temp_int, vertex_src+offset, isBigEndian);
							vt.v[j] = static_cast<float>(temp_int);
						break;

						case type::FLOAT32:
							type_cast_float<float>(&tmp_float, vertex_src+offset, isBigEndian);
							vt.v[j] = tmp_float;
						break;

						default:
						break;
					}
				}

				offset += fields[j].size;
			}
		}
		delete[] vertex_src;
		char v_indices[32];
		//Reading face data
		//TODO: A problem existed here, please resolve it
		for(int i=0; i<face_count; i++) {
			if(vertex_per_face_type == CODE_UCHAR) {
				//Firstly, read the number of vertex in face i
				file.read((char*)&temp_char, sizeof(unsigned char));
				v_per_face = static_cast<int>(temp_char);

				//then, read vertex indices into v_indices, this is an array of chars
				file.read(v_indices, sizeof(int) * v_per_face);

				//Now, we set the face properties
				face.vertex_count = v_per_face;
				for(int j=0; j<v_per_face; j++) {
					type_cast<int>(&vertex_index, v_indices+(j * sizeof(int)), isBigEndian);
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
		}
	}
}
