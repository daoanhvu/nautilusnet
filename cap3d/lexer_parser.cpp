#include "utils.h"
#include "lexer_parser.h"
#include <cstring>
#include <iostream>
#include <fstream>
#include <sstream>

using namespace std;

int startWith(char ch, string str) {
	int i = 0;
	int len = str.length();
	while( (i<len) && (str[i]==' ' || str[i]=='\t')) {
		i++;
	}
	if( (i<len) && (str[i] == ch) )
		return 1;

	return 0;
}

int parse_config_line(string line, vector<Token> &v) {
	int error = 0;
	istringstream str(line);
	string tk;
	float val;

	//cout << line << endl;
	while(!str.eof()) {
		str >> tk;
		if(tk == "lightpos1") {
			Token t(CODE_LIGHT_POS1);
			v.push_back(t);
		} else if(tk == "lightpos2") {
			Token t(CODE_LIGHT_POS2);
			v.push_back(t);
		} else if(tk == "scale") {
			Token t(CODE_SCALE);
			v.push_back(t);
		} else if(tk == ",") {
			//Do nothing here
		}	else if(tk == "window_width") {
			Token t(CODE_WINDOW_WIDTH);
			v.push_back(t);
		} else if(tk == "window_height") {
			Token t(CODE_WINDOW_HEIGHT);
			v.push_back(t);
		} else if(tk == "background") {
			Token t(CODE_BACKGROUND);
			v.push_back(t);
		} else if(tk == "=") {
			//Token t(CODE_EQUAL_SIGN);
			//v.push_back(t);
		} else if( isNumber(tk, val) ) {
			// cout << line << " NUmber: " << val << endl;
			Token t(CODE_NUMBER, val);
			v.push_back(t);
		} else {
      //ERROR
		}
	}
	return error;
}

int read_configuration(const char *filename, Configuration &config) {
  ifstream f(filename);
  string line;
  vector<Token> tokens(0);
  int size;
  if(f.fail()) {
    cout << "Failed to open this file " << filename << endl;
    return 1;
  }

  while(!f.eof()) {
    std::getline(f, line);
    tokens.clear();

    if( (line.length()<=3) || startWith('#', line) )
      continue;

    parse_config_line(line, tokens);
    size = tokens.size();
    if(tokens[0].code == CODE_LIGHT_POS1) {
      config.lightpos1[0] = tokens[1].value;
      config.lightpos1[1] = tokens[2].value;
      config.lightpos1[2] = tokens[3].value;
    } else if(tokens[0].code == CODE_LIGHT_POS2) {
      config.lightpos2[0] = tokens[1].value;
      config.lightpos2[1] = tokens[2].value;
      config.lightpos2[2] = tokens[3].value;
    } else if(tokens[0].code == CODE_WINDOW_WIDTH) {
      config.window_width = tokens[1].value;
    } else if(tokens[0].code == CODE_WINDOW_HEIGHT) {
      config.window_height = tokens[1].value;
    } else if(tokens[0].code == CODE_SCALE) {
      config.scale_factor = tokens[1].value;
    } else if(tokens[0].code == CODE_BACKGROUND) {
      config.background[0] = tokens[1].value;
      config.background[1] = tokens[2].value;
      config.background[2] = tokens[3].value;
    } else if( (tokens[0].code == CODE_NUMBER) && (tokens[1].code == CODE_NUMBER) ) {
      glm::vec3 pos = glm::vec3(tokens[0].value, tokens[1].value, tokens[2].value);
      config.camera_positions.push_back(pos);
    }
  }
  f.close();
  return 0;
}
