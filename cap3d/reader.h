#ifndef __READER_H_
#define __READER_H_

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include "model3d.h"
#include "common_cap3d.h"
#include "lexer_parser.h"

using namespace std;

typedef struct tagProp {
	string name;
	int datatype;
} PlyProperty;

struct OpenFileException : public exception {
   const char * what () const throw () {
      return "Could not open this file.";
   }
};

class Reader {
	public:
		// vector<PlyProperty> properties;

	public:
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
