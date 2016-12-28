#include <iostream>
#include <fstream>
#include <string>
#include <stdlib.h>
//#include "fmat.hpp"
#include "nautilusnet.h"

//using namespace gm;
using namespace std;

int readInputByIndex(int index, const char *filename, int &ftSize, double **X, double &y) {
	//ifstream f("/cygdrive/d/data/coursera_data.data", ios::binary);
	ifstream f(filename, ios::binary);
	
	int i, m;
	char tmp;
	
	if( f.fail() ) {
		return 0;
	}
	
	f.read((char*)&m, sizeof(int));
	//cout << "Number of examples: " << m << endl;
	f.read((char*)&ftSize, sizeof(int));
	//cout << "Number of features: " << ftSize << endl;
	
	*X = new double[ftSize];
	int offs = index * ((ftSize * 8) + 1);
	
	f.seekg(offs + 8);
	
	for(i=0; i<ftSize; i++) {
		f.read((char*)&(*X)[i], sizeof(double));
	}
	f.read((char*)&tmp, sizeof(char));
	y = (double)tmp;
	f.close();
	
	return 1;
}

int readTheta(int layer_index, const char *filename, int &row, int &col, double **X) {
	ifstream f(filename, ios::binary);
	int i;
	char tmp;
	
	if( f.fail() ) {
		return 0;
	}
	
	f.read((char*)&row, sizeof(int));
	//cout << "Number of examples: " << m << endl;
	f.read((char*)&col, sizeof(int));
	//cout << "Number of features: " << ftSize << endl;
	int size = row * col;
	*X = new double[size];
	f.seekg(8);
	for(i=0; i<size; i++) {
		f.read((char*)&(*X)[i], sizeof(double));
	}
	f.close();
	
	return 1;
}



int main(int argc, char **args) {
	
	if(argc <= 1) {
		cout << "Not enough parameters." << endl;
		cout << "Program exit." << endl;
		return 1;
	}
	
	int i, number_output_class = 10;
	int ftSize, row1, col1;
	int row2, col2;
	int read1, read2;
	NautilusNet *aNet;
	double *X = NULL;
	double *theta1 = NULL;
	double *theta2 = NULL;
	double y;
	double *t;
	double error;
	
	//if(readInputByIndex(0, args[1], ftSize, &X, y)) {
	if(readInputByIndex(0, "/cygdrive/d/data/coursera_data.data", ftSize, &X, y)) {
		//cout << "Address data array after function read " << (void*)&X << endl;
		//cout << "Number of size: " << ftSize << "; value X[70]: " << X[70] << "; digit: " << y << endl;
		
		read1 = readTheta(1, "/cygdrive/d/data/coursera_theta1.data", row1, col1, &theta1);
		if(read1) {
			cout << "Read theta1 done!" << endl;
			read2 = readTheta(2, "/cygdrive/d/data/coursera_theta2.data", row2, col2, &theta2);
			if(read2) {
				cout << "Read theta2 done!" << endl;
				t = new double[number_output_class];
				aNet = new NautilusNet(3, 400, 25, number_output_class);
				
				aNet->setWeights(0, theta1);
				aNet->setWeights(1, theta2);
				
				for(i=0; i<number_output_class; i++) {
					t[i] = 0.0;
					if( (i+1)==(int)y) {
						t[i] = 1.0;
					}
				}
				
				error = aNet->forward(X, t, 0.01);
				
				cout << "Error = " << error << endl;
				
				delete t;
				delete aNet;
				delete theta2;
			}
			delete theta1;
		}
		delete X;
	}
	return 0;
}