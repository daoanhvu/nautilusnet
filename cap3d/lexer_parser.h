#ifndef _LEXER_PARSER_H_
#define _LEXER_PARSER_H_

#include <string>
#include <vector>
#include <glm/glm.hpp>

#include "common/defines.h"

//Codes for config file starting from 150
#define CODE_EQUAL_SIGN		150
#define CODE_LIGHT_POS1		151
#define CODE_LIGHT_POS2		152
#define CODE_BACKGROUND		153
#define CODE_SCALE	        154
#define CODE_WINDOW_WIDTH	155
#define CODE_WINDOW_HEIGHT	156
#define CODE_OUTPUT_FOLDER	157
#define CODE_STRING			158

using namespace std;

typedef struct tagConfig {
  float lightpos1[3];
  float lightpos2[3];
  float background[3];
  float scale_factor;
  int window_width;
  int window_height;
  std::vector<glm::vec3> camera_positions;
	std::string output_folder;
} Configuration;


int parse_config_line(string line, vector<Token> &v);
int read_configuration(const char *filename, Configuration &config);

#endif
