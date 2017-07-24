#ifndef __READER_H_
#define __READER_H_

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include "model3d.h"
#include "../lexer_parser.h"

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
		virtual ~Reader() {}

		virtual int load(const char *filename, float scale, Model3D &){
			return -1;
		};
		virtual Model3D* load(const char *filename, float scale){
			return NULL;
		};
		/*
			Load without scaling
		*/
		virtual Model3D* load(const char *filename){
			return NULL;
		};
		virtual void load(const char *filename, Model3D &){};
		virtual int save(const Model3D *, const char *filename){
			return -1;
		};
		virtual int parse_line2(string line, vector<Token> &v) = 0;
};

#endif
