
#include <plyfile.h>
#include <cstring>
#include <utils.h>
#include <glm/glm.hpp>

PlyFile::PlyFile() {
	float_stride = 0;
}

PlyFile::PlyFile(const char *filename) {
	ifstream f(filename);
	char line[128];
	int i;
	float x, y, z;

	int vertex_count = 0;
	int face_count = 0;

	try {
		if(f.fail()) {
			throw new OpenFileException;
		}
		//read content here

		f.close();
	} catch(const OpenFileException &e) {
		std::cout << e.what();
	}
}

PlyFile::PlyFile(string filename) {
	float_stride = 0;
}

float* PlyFile::getVertexBuffer(unsigned int &nc) {
	int i, j;
	nc = vertices.size();
	unsigned int n = nc * float_stride;
	float *buf = new float[n];
	int offs = 0;
	unsigned int normal_size_in_byte = sizeof(float) * float_stride;
	for(i=0; i<nc; i++) {
		std::memcpy(buf + offs, vertices[i].v, normal_size_in_byte);
		offs += float_stride;
	}
	return buf;
}

/*
	Params
	Return
*/
float* PlyFile::getNormalBuffer(unsigned int &nc) {
	int i, j;
	nc = vertices.size();
	unsigned int n = nc * 3;
	float *buf = new float[n];
	int offs = 0;
	unsigned int normal_size_in_byte = sizeof(float) * 3;
	for(i=0; i<nc; i++) {
		std::memcpy(buf + offs, vertices[i].normal, normal_size_in_byte);
		offs += 3;
	}
	return buf;
}

void PlyFile::getBBox(BBox3d &bbox) {
	bbox.minx = 999999.0f;
	bbox.maxx = -9999.0f;
	bbox.miny = 999999.0f;
	bbox.maxy = -9999.0f;
	bbox.minz = 999999.0f;
	bbox.maxz = -9999.0f;

	int vertex_count = vertices.size();

	for(int i=0; i<vertex_count; i++) {
		if(vertices[i].v[0] < bbox.minx) {
			bbox.minx = vertices[i].v[0];
		}

		if(vertices[i].v[0] > bbox.maxx) {
			bbox.maxx = vertices[i].v[0];
		}

		if(vertices[i].v[1] < bbox.miny) {
			bbox.miny = vertices[i].v[1];
		}

		if(vertices[i].v[1] > bbox.maxy) {
			bbox.maxy = vertices[i].v[1];
		}

		if(vertices[i].v[2] < bbox.minz) {
			bbox.minz = vertices[i].v[2];
		}

		if(vertices[i].v[2] > bbox.maxz) {
			bbox.maxz = vertices[i].v[2];
		}
	}
}

int PlyFile::parse_line2(string line, vector<Token> &v) {
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

int PlyFile::load(const char *filename, float scale) {
	ifstream f(filename);
	string line;
	vector<Token> tokens(0);
	bool read_vertex;
	int size, v_per_face;
	float tmp;
	int vertex_index, vertex_count, face_count;

	if(f.fail()) {
		cout << "Failed to open this file " << filename << endl;
		return 1;
	}

	this->float_stride = 0;

	while(!f.eof()) {
		std::getline(f, line);
		tokens.clear();
		parse_line2(line, tokens);
		size = tokens.size();
		if(size == 1 && tokens[0].code == CODE_END_HEADER) {
			//start read vertices
			read_vertex = true;

			this->vertices.reserve(vertex_count);

			for(int i=0; i<vertex_count; i++) {
				std::getline(f, line);
				istringstream str(line);
				Vertex vt;
				vt.v = new float[float_stride];
				vt.count = 0;
				for(int j=0; j<float_stride; j++) {
					str >> tmp;
					vt.v[j] = tmp / scale;
				}
				vertices.push_back(vt);
			}

			this->faces.reserve(face_count);
			//Now read face
			for(int i=0; i<face_count; i++) {
				std::getline(f, line);
				istringstream str1(line);
				Face face;
				str1 >> v_per_face;
				face.vertex_count = v_per_face;
				//cout << "Face line: " << line << " v_per_face: "<< v_per_face << endl;
				for(int j=0; j<v_per_face; j++) {
					str1 >> vertex_index;
					face.vertex_indices[j] = vertex_index;
					vertices[vertex_index].face_indices[vertices[vertex_index].count++] = i;
				}
				faces.push_back(face);
			}
		} else if(tokens[0].code == CODE_PROPERTY) {
			if(tokens[1].code == CODE_FLOAT32) {
				if(tokens[2].code == CODE_COORD_X || (tokens[2].code == CODE_COORD_Y) || tokens[2].code == CODE_COORD_Z ) {
					this->float_stride++;
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

	return 0;
}

/*
	Calculate normal vector for faces
*/
int PlyFile::add_normal_vectors() {
	int i;
	int vxcount;
	int *v;
	glm::vec3 e1, e2;
	glm::vec3 normal;
	float v0a, v0b, v0c;
	float v1a, v1b, v1c;
	float v2a, v2b, v2c;
	int face_count = faces.size();

	for(i=0; i<face_count; i++) {
		vxcount = faces[i].vertex_count;
		v = faces[i].vertex_indices;

		v0a = vertices[v[0]].v[0];
		v0b = vertices[v[0]].v[1];
		v0c = vertices[v[0]].v[2];

		v1a = vertices[v[1]].v[0];
		v1b = vertices[v[1]].v[1];
		v1c = vertices[v[1]].v[2];

		v2a = vertices[v[2]].v[0];
		v2b = vertices[v[2]].v[1];
		v2c = vertices[v[2]].v[2];

		e1 = glm::vec3(v1a - v0a, v1b - v0b, v1c - v0c);
		e2 = glm::vec3(v2a - v0a, v2b - v0b, v2c - v0c);

		normal = glm::cross(e1, e2);
		normal = glm::normalize(normal);
		faces[i].normal[0] = normal[0];
		faces[i].normal[1] = normal[1];
		faces[i].normal[2] = normal[2];
		//cout << "Normal vector: " << normal[0] << ", " << normal[1] << ", " << normal[2] << endl;
	}

	vxcount = vertices.size();
	for(i=0; i<vxcount; i++) {
		normal = glm::vec3(0, 0, 0);
		for(int j=0; j<vertices[i].count; j++) {
			normal[0] += faces[vertices[i].face_indices[j]].normal[0];
			normal[1] += faces[vertices[i].face_indices[j]].normal[1];
			normal[2] += faces[vertices[i].face_indices[j]].normal[2];
		}
		normal = glm::normalize(normal);
		vertices[i].normal[0] = normal[0];
		vertices[i].normal[1] = normal[1];
		vertices[i].normal[2] = normal[2];
	}
	return 0;
}
