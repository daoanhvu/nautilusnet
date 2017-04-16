#include <utils.h>
#include <stdlib.h>


int isNumber(string s, float &val) {
	int i, start = 0;
	char isFloatingPoint = 0;
	long floating = 1;
	char negative = 1;
	int len = s.length();

	error = -1;

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
				return 1;
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

	return 0;
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
