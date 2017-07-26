#ifndef __PLYREADER_H_
#define __PLYREADER_H_

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include "plymodel3d.h"
#include "reader.h"
#include "../lexer_parser.h"

using namespace std;

class PLYReader: public Reader {
	protected:
		
	public:
		virtual ~PLYReader() {}

		virtual int load(const char *filename, float scale, Model3D &);
		virtual Model3D* load(const char *filename, float scale);
		/*
			Load without scaling
		*/
		virtual Model3D* load(const char *filename);
		virtual void load(const char *filename, Model3D &);
		virtual int save(const Model3D *, const char *filename);
		int parse_line2(string line, vector<Token> &v);
};

#endif
