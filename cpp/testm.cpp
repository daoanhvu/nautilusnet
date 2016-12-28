#include <iostream>
#include <fstream>
#include <string>
#include <stdlib.h>
#include "fmat.hpp"

using namespace gm;
using namespace std;

void testMatrix() {
	double vs[3] = {3.6, 1.2, 2.0};
	double ms[6] = {5.2, 2.0, 1.4, 0.7, 4.1, 0.5};
	FMat<double> m(ms, 2, 3);
	Vec<double> v(vs, 3);

	cout << m << endl;
	cout << v << endl;
	
	//Vec<double> t = m * v;
	Vec<double> t;
	t = m * v;
	
	cout << t << endl;
}

void testReadFile(int argc, char **args) {
	ifstream f("/cygdrive/d/data/coursera_X.data", ios::binary);
	
	int index = (int)strtol(args[1], NULL, 10);
	
	int m, i, j;
	int ftSize;
	double tmp;
	double X[400];
	
	f.read((char*)&m, sizeof(int));
	cout << "Number of examples: " << m << endl;
	f.read((char*)&ftSize, sizeof(int));
	cout << "Number of features: " << ftSize << endl;
	
	for(i=0; i<1; i++) {
		for(j=0; j<ftSize; j++) {
			//f.read((char*)&tmp, sizeof(double));
			//X[j] = tmp;
			f.read((char*)&X[j], sizeof(double));
			//cout << "X[" << j <<"]: " << X[j] << endl;
		}
	}
	
	cout << "X["<< index <<"] = " << X[index] << endl;
	
	f.close();
}

int main(int argc, char **args) {
	
	testReadFile(argc, args);
	
	return 0;
}