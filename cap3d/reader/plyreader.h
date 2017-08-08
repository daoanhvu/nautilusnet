#ifndef __PLYREADER_H_
#define __PLYREADER_H_

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include "plymodel3d.h"
#include "reader.h"

#ifdef __linux__
	#include <cstdint>
#endif

using namespace std;

class PLYReader: public Reader {
	protected:
		int vertex_per_face_type;
		int vertex_index_type;

		bool isBigEndian;
		int readVertextProperty(PointField &pf, int data_type);
		void readProperty(const vector<Token>& tokens, int element_type, PLYModel3D *model, int &float_stride);
		void readpoints(std::ifstream& file, unsigned int offs,
				int format_type, int vertex_count, 
				vector<Vertex>& vertices, vector<Face>& faces, int face_count, int float_stride);
	public:
		PLYReader(): isBigEndian(false) {
		}

		virtual ~PLYReader() {}

		virtual int load(const char *filename, float scale, Model3D &);
		virtual Model3D* load(const char *filename, float scale, bool should_add_normal, bool should_add_color);
		/*
			Load without scaling
		*/
		virtual Model3D* load(const char *filename);
		virtual void load(const char *filename, Model3D &);
		virtual int save(const Model3D *model1, const char *filename, int format);
		int parse_line2(string line, vector<Token> &v);		
};

#endif
