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

struct OpenFileException : public exception {
   const char * what () const throw () {
      return "Could not open this file.";
   }
};

namespace type {
	enum FieldType {
		INT8,
		UINT8,
		INT16,
		UINT16,
		INT32,
		UINT32,
		FLOAT32,
		FLOAT64
	};
}

enum FieldCode {
		X,
		Y,
		Z,
		NORMAL_X,
		NORMAL_Y,
		NORMAL_Z,
		RGB,
		RGBA,
		RED,
		GREEN,
		BLUE,
		ALPHA,
		IMX,
		IMY
};

typedef struct tagPointField {
	FieldCode code;
	type::FieldType type;
	short index;

	public:
		tagPointField(){}
		tagPointField(FieldCode c, type::FieldType t){
			code = c;
			type = t;
		}
} PointField;

class Reader {
	protected:
		vector<PointField> fields;
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
