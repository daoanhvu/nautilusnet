#include "utils.h"
#include <stdlib.h>
#include <iostream>

#ifdef __linux__
	#include <cstring>
#endif

using namespace std;

/**
	Convert n to string and store in s from offs

*/
int itostr(int n, char *s, int offs) {
		int d, len = 0;
		int l2, t = n;
		int tmp;

		if(n == 0) {
			s[offs] = '0';
			s[offs + 1] = 0;
			return (offs + 1);
		}

		while( t > 0 ) {
			d = t % 10;
			t = t / 10;
			s[offs + len] = d + 48;
			len++;
		}
		s[offs+len] = '\0';
		
		l2 = len >> 1;
		for(t=0; t<l2; t++) {
			tmp = s[offs+t];
			s[offs + t] = s[offs + len-t-1];
			s[offs + len-t-1] = tmp;
		}
		
		return offs + len;
}

int isNumber(string str, float &val) {
	int i, start = 0;
	char isFloatingPoint = 0;
	long floating = 1;
	char negative = 1;
	int len = str.length();
	val = 0;

	if(str[start] == '-') {
		negative = -1;
		start++;
	}

	for(i=start; i<len; i++) {
		if(str[i]=='\0')
			return 0;

		if((str[i]<48) || (str[i]>57)){
			if( str[i] == 46 && isFloatingPoint==0)
				isFloatingPoint = 1;
			else{
				/*printf(" Floating point ERROR F\n");*/
				val *= negative;
				return 0;
			}
		} else {
			if(isFloatingPoint){
				floating *= 10;
				val = val + (float)(str[i] - 48)/floating;
			}else
				val = val * 10 + (str[i] - 48);
		}
	}
	val *= negative;

	return 1;
}

int isDigit(char c) {
	if (c >= 48 && c <= 57)
		return 1;
	return 0;
}

float parseFloat(string str, int start, int end, int &error){
	int i;
	float val = 0;
	char isFloatingPoint = 0;
	long floating = 1;
	char negative = 1;

	error = -1;

	if(str[start] == '-') {
		negative = -1;
		start++;
	}

	for(i=start; i<end; i++) {

		if(str[i]=='\0')
			return val;

		if((str[i]<48) || (str[i]>57)){
			if( str[i] == 46 && isFloatingPoint==0)
				isFloatingPoint = 1;
			else{
				error = 1;
				/*printf(" Floating point ERROR F\n");*/
				return val;
			}
		} else {
			if(isFloatingPoint){
				floating *= 10;
				val = val + (float)(str[i] - 48)/floating;
			}else
				val = val * 10 + (str[i] - 48);
		}
	}
	error = 0;
	return val * negative;
}

/**
	Check if s1 existed in s, return the position of the first character of s1 in s, return -1 in case of
	s1 is not existed in s.
	CAUTION: both s and s1 must be ended with end-string character '\0'

	BRUTE-FORCE????
*/
int find(const char* s, const char* s1) {
	int i = 0;
	int k = 0;

	while(s[i] != 0) {
		k = 0;
		while( (s[i+k] == s1[k]) && (s1[k] != 0) ) {
			k++;
		}
		if(s1[k] == 0) {
			//found!
			return i;
		}
		i++;
	}

	return -1;
}

/**
	Build a file name with format: <classname>_<poseNumber>.jpg
*/
void getFrameName(int idx, const char *cls, char *name) {
	int i, len, cls_len = strlen(cls);
	memcpy(name, cls, cls_len);
	name[cls_len++] = '_';
	len = itostr(idx, name, cls_len);
	name[len] = '.';
	name[len+1] = 'j';
	name[len+2] = 'p';
	name[len+3] = 'g';
	name[len+4] = '\0';
}
