#ifndef _UTILS_H_
#define _UTILS_H_

#include <string>

using namespace std;

int itostr(int, char *);
int isNumber(string str, float &val);
int isDigit(char c);
float parseFloat(string str, int start, int end, int &error);

#endif
