#ifndef _PCDREADER_H_
#define _PCDREADER_H_

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include "reader.h"
#include "pcdmodel3d.h"
#include "../lexer_parser.h"

//define token codes
#define CODE_PCD_VERSION	1
#define CODE_PCD_FIELDS     2
#define CODE_PCD_SIZE       3
#define CODE_PCD_TYPE       4
#define CODE_PCD_COUNT      5
#define CODE_PCD_WIDTH      6
#define CODE_PCD_HEIGHT     7
#define CODE_PCD_VIEWPOINT  8
#define CODE_PCD_POINTS     9
#define CODE_PCD_DATA      10
#define CODE_PCD_X       	11
#define CODE_PCD_Y 		    12
#define CODE_PCD_Z 	        13
#define CODE_PCD_RGB 	    14
#define CODE_PCD_RGBA 	    15
#define CODE_PCD_NORM_X	    16
#define CODE_PCD_NORM_Y		17
#define CODE_PCD_NORM_Z     18
#define CODE_PCD_IMX		19
#define CODE_PCD_IMY		20
#define CODE_PCD_ASCII		21
#define CODE_PCD_FLOAT		22
#define CODE_PCD_SIGNED		23
#define CODE_PCD_UNSIGNED	24
#define CODE_PCD_BINARY		25

class PCDReader: public Reader {
private:
protected:
		void readpoints(std::ifstream& file, unsigned int offs, int format_type, int vertex_count, vector<Vertex>& vertices, int float_stride);
public:
	PCDReader() {
	}

	virtual ~PCDReader(){};
	
	int parse_line2(string line, vector<Token> &v);
	virtual Model3D* load(const char *filename, float scale);
};

#endif