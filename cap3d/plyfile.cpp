
#include <plyfile.h>

#include <utils.h>
#include <glm/glm.hpp>

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
		//read content here

		f.close();
	} catch(const OpenFileException &e) {
		std::cout << e.what();
	}
}

PlyFile::PlyFile(string filename) {
	vertices = 0;
	vertex_count = 0;
	face_count = 0;
	float_stride = 0;
}

void PlyFile::getBBox(BBox3d &bbox) {
	bbox.minx = 999999.0f;
	bbox.maxx = -9999.0f;
	bbox.miny = 999999.0f;
	bbox.maxy = -9999.0f;
	bbox.minz = 999999.0f;
	bbox.maxz = -9999.0f;

	int k;
	for(int i=0; i<vertex_count; i++) {
		k = i * float_stride;
		if(vertices[k] < bbox.minx) {
			bbox.minx = vertices[k];
		}

		if(vertices[k] > bbox.maxx) {
			bbox.maxx = vertices[k];
		}

		if(vertices[k+1] < bbox.miny) {
			bbox.miny = vertices[k+1];
		}

		if(vertices[k+1] > bbox.maxy) {
			bbox.maxy = vertices[k+1];
		}

		if(vertices[k+2] < bbox.minz) {
			bbox.minz = vertices[k+2];
		}

		if(vertices[k+2] > bbox.maxz) {
			bbox.maxz = vertices[k+2];
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

int PlyFile::load2(const char *filename) {
	ifstream f(filename);
	string line;
	vector<Token> tokens(0);
	bool read_vertex;
	int size, v_per_face;
	float tmp;
	int vertex_index;

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

			this->vertices = new float[float_stride * vertex_count];

			for(int i=0; i<vertex_count; i++) {
				std::getline(f, line);
				istringstream str(line);
				for(int j=0; j<float_stride; j++) {
					str >> tmp;
					this->vertices[i * float_stride + j] = tmp;
				}
			}

			coVertices.reserve(vertex_count);

			this->faces = new Face[this->face_count];
			//Now read face
			for(int i=0; i<face_count; i++) {
				std::getline(f, line);
				istringstream str1(line);
				str1 >> v_per_face;
				this->faces[i].vertex_count = v_per_face;
				//cout << "Face line: " << line << " v_per_face: "<< v_per_face << endl;
				for(int j=0; j<v_per_face; j++) {
					str1 >> vertex_index;
					//cout << vertex_index << " ";
					this->faces[i].vertex_indices[j] = vertex_index;
					coVertices[vertex_index].face_indices[coVertices[vertex_index].count++] = i;
				}
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
				this->vertex_count = (int)(tokens[2].value);
			} else if(tokens[1].code == CODE_FACE) {
				this->face_count = (int)(tokens[2].value);
			}
		}
	}

	f.close();

	return 0;
}

/*
	Deprecated
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

			//this->vertices = new float[vertex_count * properties.size()];
			this->vertices = new float[vertex_count * 3];
			int offs;
			for(i=0; i<this->vertex_count; i++) {
				// offs = i * properties.size();
				offs = i * 3;
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

				if(i<10)
					printf("(%f %f %f)\n", vertices[offs], vertices[offs+1], vertices[offs+2]);
			}
			cout << "Start reading faces\n";
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
				// cout << "Number of vertex " << this->vertex_count << std::endl;
			} else if(tk2 == "face") {
				ls >> face_count;
				// cout << "Number of faces: " << face_count << std::endl;
			}
		} else if(token == "property") {
			string tk3;
			ls >> tk3;
			if(tk3 == "float32") {
				float_stride++;
			}
			// cout << "Property: " << tk3 << endl;
		}
		//cout << line << std::endl;
	}

	f.close();
	return 0;
}

int PlyFile::add_normal_vectors() {
	int i;
	int vxcount;
	int *v;
	glm::vec3 e1, e2;
	glm::vec3 normal;
	float v0a, v0b, v0c;
	float v1a, v1b, v1c;
	float v2a, v2b, v2c;
	for(i=0; i<face_count; i++) {
		vxcount = faces[i].vertex_count;
		v = faces[i].vertex_indices;

		v0a = vertices[v[0] * float_stride];
		v0b = vertices[v[0] * float_stride + 1];
		v0c = vertices[v[0] * float_stride + 2];

		v1a = vertices[v[1] * float_stride];
		v1b = vertices[v[1] * float_stride + 1];
		v1c = vertices[v[1] * float_stride + 2];

		v2a = vertices[v[2] * float_stride];
		v2b = vertices[v[2] * float_stride + 1];
		v2c = vertices[v[2] * float_stride + 2];

		e1 = glm::vec3(v1a - v0a, v1b - v0b, v1c - v0c);
		e2 = glm::vec3(v2a - v0a, v2b - v0b, v2c - v0c);

		normal = glm::cross(e1, e2);
		normal = glm::normalize(normal);
		faces[i].normal[0] = normal[0];
		faces[i].normal[1] = normal[1];
		faces[i].normal[2] = normal[2];
		//cout << "Normal vector: " << normal[0] << ", " << normal[1] << ", " << normal[2] << endl;
	}
	return 0;
}
