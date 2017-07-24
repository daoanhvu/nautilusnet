
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
		} else if(tk == "comment" || tk == "converted" || tk == "from" || tk == "OBJ" ) {

		}
	}

	return error;
}

//TODO: Not implemented yet
int PLYReader::load(const char *filename, float scale, Model3D &model) {
	return -1;
}

Model3D* PLYReader::load(const char *filename, float scale) {
	ifstream f(filename);
	string line;
	vector<Token> tokens(0);
	bool read_vertex;
	int size, v_per_face;
	float tmp;
	int vertex_index, vertex_count, face_count;
	PLYModel3D *result;
	if(f.fail()) {
		cout << "Failed to open this file " << filename << endl;
		return NULL;
	}

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
			// cout << "[DEBUG] vertex count: " << vertex_count << endl;
			for(int i=0; i<vertex_count; i++) {
				std::getline(f, line);
				istringstream str(line);
				Vertex vt;
				vt.v = new float[float_stride];
				vt.count = 0;
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

					if(vertices[vertex_index].count >= vertices[vertex_index].log_face_size) {
						vertices[vertex_index].log_face_size += 8;
						face_indices = new int[vertices[vertex_index].log_face_size];
						std::memcpy(face_indices, vertices[vertex_index].face_indices, sizeof(int) * vertices[vertex_index].count);
						delete[] vertices[vertex_index].face_indices;
						vertices[vertex_index].face_indices = face_indices;
					}
					vertices[vertex_index].face_indices[vertices[vertex_index].count++] = i;
				}
				faces.push_back(face);
			}
		} else if(tokens[0].code == CODE_PROPERTY) {
			if(tokens[1].code == CODE_FLOAT32) {
				if(tokens[2].code == CODE_COORD_X || (tokens[2].code == CODE_COORD_Y) || tokens[2].code == CODE_COORD_Z ) {
					float_stride++;
				}
			}
		} else if(tokens[0].code == CODE_ELEMENT) {
			if(tokens[1].code == CODE_VERTEX) {
				//cout << line << endl;
				vertex_count = (int)(tokens[2].value);
			} else if(tokens[1].code == CODE_FACE) {
				face_count = (int)(tokens[2].value);
			}
		}
	}

	f.close();
	result = new PLYModel3D(vertices, faces, float_stride);

	//Need safe type-casting???? NO
	result->scaleToFit(scale);
	((PLYModel3D*)result)->add_normal_vectors();

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
	int size, v_per_face;
	float tmp;
	int vertex_index, vertex_count, face_count;
	Model3D *result;
	if(f.fail()) {
		cout << "Failed to open this file " << filename << endl;
		return NULL;
	}

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
				vt.count = float_stride;
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

					if(vertices[vertex_index].count >= vertices[vertex_index].log_face_size) {
						vertices[vertex_index].log_face_size += 8;
						face_indices = new int[vertices[vertex_index].log_face_size];
						std::memcpy(face_indices, vertices[vertex_index].face_indices, sizeof(int) * vertices[vertex_index].count);
						delete[] vertices[vertex_index].face_indices;
						vertices[vertex_index].face_indices = face_indices;
					}
					vertices[vertex_index].face_indices[vertices[vertex_index].count++] = i;
				}
				faces.push_back(face);
			}
		} else if(tokens[0].code == CODE_PROPERTY) {
			if(tokens[1].code == CODE_FLOAT32) {
				if(tokens[2].code == CODE_COORD_X || (tokens[2].code == CODE_COORD_Y) || tokens[2].code == CODE_COORD_Z ) {
					float_stride++;
				}
			}
		} else if(tokens[0].code == CODE_ELEMENT) {
			if(tokens[1].code == CODE_VERTEX) {
				//cout << line << endl;
				vertex_count = (int)(tokens[2].value);
			} else if(tokens[1].code == CODE_FACE) {
				face_count = (int)(tokens[2].value);
			}
		}
	}

	f.close();

	result = new PLYModel3D(vertices, faces, float_stride);

	return result;
}

int PLYReader::save(const Model3D *model, const char *filename) {
	return 0;
}
