
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

void PlyFile::parse_line(string line, vector<Token> &v) {
	int sl = line.length();
	int i = 0;
	int error;
	while( i<sl ) {
		if(line[i]=='f' && line[i+1]=='a' && line[i+2]=='c' && line[i+3] == 'e') {
			Token tk(CODE_FACE);
			v.push_back(tk);
			i += 4;
		} else if(line[i] == 'p') {
			if(line[i+1] == 'l' && line[i+2] == 'y') {
				Token tk(CODE_PLY);
				v.push_back(tk);
				i += 3;
			} else if(line[i+1] == 'r' && line[i+2] == 'o' && line[i+3] == 'p' && line[i+4]=='e'
		 			&& line[i+5]=='r' && line[i+6] == 't' && line[i+7]=='t' && line[i+8]=='y') {
				Token tk(CODE_PROPERTY);
				v.push_back(tk);
				i += 9;
			}
		} else if(line[i]=='e') {
			if(line[i+1] == 'l' && line[i+2]=='e' && line[i+3]=='m' && line[i+4]=='e' && line[i+5]=='n' && line[i+6]=='t') {
				Token tk(CODE_ELEMENT);
				v.push_back(tk);
				i += 7;
			} else if(line[i+1] == 'n' && line[i+2]=='n' && line[i+3]=='_' && line[i+4]=='h' && line[i+5]=='e'
						&& line[i+6]=='a' && line[i+7]=='d' && line[i+8]=='e' && line[i+9]=='r') {
				Token tk(CODE_END_HEADER);
				v.push_back(tk);
				i += 10;
			}
		} else if(line[i] == 'v' && line[i+1] == 'e' && line[i+2]=='r' && line[i+3]=='t' && line[i+4]=='e' && line[i+5] == 'x') {
			if(line[i+6]==' ') {
				Token tk(CODE_VERTEX);
				v.push_back(tk);
				i += 6;
			} else if(line[i+6] == '_' && line[i+7] == 'i' && line[i+8] == 'n' && line[i+9] == 'd' && line[i+10]=='i'
						&& line[i+11]=='c' && line[i+12]=='e' && line[i+13]=='s') {
				Token tk(CODE_VERTEX_INDICES);
				v.push_back(tk);
				i += 14;
			}
		} else if(isDigit(line[i])) {
			int floatingPoint = 0;
			int k;
			for(k = i+1; k < sl; k++) {
				if(!isDigit(line[k])) {
					if(line[k] == '.') {
						//check if we got a floating point
						if(floatingPoint) {
							//ERROR: the second floating point found
						}
						floatingPoint = 1;
					} else {
						float val = parseFloat(line, i, k, error);
						Token tk(CODE_NUMBER, val);
						v.push_back(tk);
						i = k;
						break;
					}
				}
			}

			if(i < k) {
				float val = parseFloat(line, i, k, error);
				Token tk(CODE_NUMBER, val);
				v.push_back(tk);
				i = k;
			}
		} else if(line[i] == 'x') {
			Token tk(CODE_COORD_X);
			v.push_back(tk);
			i++;
		} else if(line[i] == 'y') {
			Token tk(CODE_COORD_Y);
			v.push_back(tk);
			i++;
		} else if(line[i] == 'z') {
			Token tk(CODE_COORD_Z);
			v.push_back(tk);
			i++;
		} else if(line[i] == 'u' && line[i+1]=='i' && line[i+2]=='n' && line[i+3]=='t' && line[i+4]=='3' && line[i+5] =='2' ) {
			Token tk(CODE_UNIT32);
			v.push_back(tk);
			i += 6;
		}
		else {
			i++;
		}
	}
}

/*
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
	}
	return 0;
}
