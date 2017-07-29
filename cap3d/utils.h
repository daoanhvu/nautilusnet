#ifndef _UTILS_H_
#define _UTILS_H_

#include <string>

using namespace std;

int itostr(int, char *, int);
int isNumber(string str, float &val);
int isDigit(char c);
float parseFloat(string str, int start, int end, int &error);

/**
	Check if s1 existed in s, return the position of the first character of s1 in s, return -1 in case of
	s1 is not existed in s.
	CAUTION: both s and s1 must be ended with end-string character '\0'
*/
int find(const char* s, const char* s1);

void getFrameName(int idx, const char *cls, char *name);

#endif
