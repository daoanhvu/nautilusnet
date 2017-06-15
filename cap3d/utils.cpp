#include "utils.h"
#include <stdlib.h>
#include <iostream>

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
