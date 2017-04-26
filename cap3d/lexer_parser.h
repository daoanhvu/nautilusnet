#ifndef _LEXER_PARSER_H_
#define _LEXER_PARSER_H_

#include <string>
#include <vector>
#include <glm/glm.hpp>

//define token codes
#define CODE_PLY 			        1
#define CODE_PROPERTY 	      2
#define CODE_NUMBER 		      3
#define CODE_COORD_X 	        4
#define CODE_COORD_Y 	        5
#define CODE_COORD_Z 	        6
#define CODE_FLOAT32 	        7
#define CODE_LIST 			      8
#define CODE_FORMAT 		      9
#define CODE_FACE 			      10
#define CODE_END_HEADER       11
#define CODE_INT32 		        12
#define CODE_COMMENT 	        13
#define CODE_ELEMENT 	        14
#define CODE_VERTEX		        15
#define CODE_VERTEX_INDICES		16
#define CODE_UNIT32	          17
#define CODE_UNIT8	          18

//Codes for config file
#define CODE_EQUAL_SIGN	19
#define CODE_LIGHT_POS1	20
#define CODE_LIGHT_POS2	21
#define CODE_BACKGROUND	22
#define CODE_SCALE	         23
#define CODE_WINDOW_WIDTH	   24
#define CODE_WINDOW_HEIGHT	 25
#define CODE_OUTPUT_FOLDER	 26

using namespace std;

typedef struct tagTK {
		int code;

		//In case of code = CODE_NUMBER
		float value;

		tagTK() {
			code = 0;
			value = 0.0f;
		}
		tagTK(int cd) {
			code = cd;
			value = 0.0f;
		}
		tagTK(int cd, float v) {
			code = cd;
			value = v;
		}
} Token;

typedef struct tagConfig {
  float lightpos1[3];
  float lightpos2[3];
  float background[3];
  float scale_factor;
  int window_width;
  int window_height;
  std::vector<glm::vec3> camera_positions;
} Configuration;


int parse_config_line(string line, vector<Token> &v);
int read_configuration(const char *filename, Configuration &config);

#endif
