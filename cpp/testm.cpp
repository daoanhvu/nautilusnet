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
	FMat<double> v(vs, 3, 1);

	cout << m << endl;
	cout << v << endl;
	
	//Vec<double> t = m * v;
	FMat<double> t;
	t = m * v;
	
	cout << t << endl;
}

/**
	Test multiply transpose matrix
*/
void testMultiplyTransposeMatrix() {
	double vs[6] = {1.0, 0.7773, 0.80139, 0.77277, 0.84839, 0.84436};
	double ms[12] = {	0.23, 0.18, 0.40, 0.74, 0.12, 0.35,
						0.66, 0.27, 0.54, 0.40, 0.21, 0.5};
	FMat<double> m(ms, 2, 6);
	FMat<double> v(vs, 1, 6);

	cout << m << endl;
	cout << v << endl;
	
	FMat<double> t;
	t = v * m.transpose();
	
    cout << "M transpose: " << endl;
    cout << m.transpose() << endl;
    cout << "Result: " << endl;
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
	
	//testReadFile(argc, args);
    //testMatrix();
	testMultiplyTransposeMatrix();
	
	return 0;
}