#include <iostream>
#include <fstream>
#include <string>
#include <stdlib.h>
//#include "fmat.hpp"
#include "nautilusnet.h"

//using namespace gm;
using namespace std;

int readInputByIndex(int index, const char *filename, int ftSize, double *X, double &y) {
	//ifstream f("/cygdrive/d/data/coursera_data.data", ios::binary);
	ifstream f(filename, ios::binary);
	
	int i, m;
	char tmp;
	
	if( f.fail() ) {
		return 0;
	}
	
	int offs = index * ((ftSize * 8) + 1);
	
	f.seekg(offs + 8);
	
	for(i=0; i<ftSize; i++) {
		f.read((char*)&(X[i]), sizeof(double));
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
	f.read((char*)&col, sizeof(int));
	
	int size = row * col;
	*X = new double[size];
	f.seekg(8);
	for(i=0; i<size; i++) {
		f.read((char*)&(*X)[i], sizeof(double));
	}
	f.close();
	
	return 1;
}

int main1(int argc, char **args) {
	double X[4] = {0.0, 1.3, 1.5, 1.0};
	double y[1] = {1};
	double theta1[] = {0.01, 0.015, 0.3, 0.5, 0.1,
                0.18, 0.06, 0.1, 0.25, 0.71,
                0.14, 0.05, 0.23, 0.39, 0.2,
                0.21, 0.18, 0.54, 0.4, 0.21,
                0.17, 0.11, 0.87, 0.2, 0.09};
	double theta2[] = { 0.23, 0.18, 0.4, 0.74, 0.12, 0.35,
                        0.66, 0.27, 0.54, 0.4, 0.21, 0.5};
	double t[2];
	double error = 0.0;
	NautilusNet *aNet;
	const int number_of_labels = 2;
	
	aNet = new NautilusNet(3, 4, 5, number_of_labels);
	aNet->setWeights(0, theta1);
	aNet->setWeights(1, theta2);
	
    /*
	for(int i=0; i<number_of_labels; i++) {
		t[i] = 0.0;
		if( i==(int)y[0]) {
			t[i] = 1.0;
		}
	}
    */
    t[0] = 1.0;
    t[1] = 0.0;
    
	error = aNet->forward(X, t, 0.0);	
	cout << "error = " << error << endl;
	
	delete aNet;
	
	return 0;
}

void setX(double *X) {
	X[67] = 0.00001;
	X[69] = -0.00074;
	X[70] = -0.00813;
	X[71] = -0.01861;
	
	X[72] = -0.01874;
	X[73] = -0.01876;
	X[74] = -0.01910;
	X[75] = -0.01640;
	X[76] = -0.00378;
	X[77] = 0.00033;
	X[78] = 0.00001;
	
	X[86] = 0.00012;
	X[87] = 0.00012;
	X[88] = -0.01404;
	X[89] = -0.02845;
	X[90] = 0.08038;
	X[91] = 0.26654;
	X[92] = 0.27385;
	X[93] = 0.27873;
	X[94] = 0.27429;
	X[95] = 0.22468;
	
	X[96] = 0.02776;
	X[97] = -0.00706;
	X[98] = 0.00023;
	X[106] = -0.00033;
	X[107] = -0.01387;
	
	X[108] = 0.08157;
	X[109] = 0.38280;
	X[110] = 0.85785;
	X[111] = 1.00110;
	X[112] = 0.96971;
	X[113] = 0.93093;
	X[114] = 1.00384;
	X[115] = 0.96416;
	X[116] = 0.44926;
	X[117] = -0.00560;
	X[118] = -0.00378;
	X[123] = 0.00001;
}

int main(int argc, char **args) {
	
	if(argc <= 1) {
		cout << "Not enough parameters." << endl;
		cout << "Program exit." << endl;
		return 1;
	}
	
	//Number of examples
	int m;
	//Number of features
	int ftSize;
	
	int i, k, number_output_class = 10;
	int row1, col1;
	int row2, col2;
	int read1, read2;
	NautilusNet *aNet;
	double *X = NULL;
	double *theta1 = NULL;
	double *theta2 = NULL;
	double y;
	double *t;
	double error = 0;
	
	//Read number of example and number of features from filebuf
	ifstream f("./coursera_data/coursera_data.data", ios::binary);
	if( f.fail() ) {
		cout << "Could not read data file!" << endl;
		cout << "Program exit." << endl;
		return 2;
	}
	
	f.read((char*)&m, sizeof(int));
	cout << "Number of examples: " << m << endl;
	f.read((char*)&ftSize, sizeof(int));
	cout << "Number of features: " << ftSize << endl << "=========================" << endl << endl;
	f.close();
	
	X = new double[ftSize];
	
	read1 = readTheta(1, "./coursera_data/coursera_theta1.data", row1, col1, &theta1);
	if(!read1) {
		cout << "Failed to read theta1!" << endl;
		delete X;
		return 3;
	}
	
	read2 = readTheta(2, "./coursera_data/coursera_theta2.data", row2, col2, &theta2);
	if(!read2) {
		cout << "Failed to read theta2!" << endl;
		delete X;
		return 3;
	}
	
	cout << "Read theta1, theta2 done!" << endl << endl;
	
	t = new double[number_output_class];
	aNet = new NautilusNet(3, 400, 25, number_output_class);
	aNet->setWeights(0, theta1);
	aNet->setWeights(1, theta2);
	
	//Test the first training example
	m = 2;
	
	k = 0;
	while( k<m ) {
		if(readInputByIndex(k, "./coursera_data/coursera_data.data", ftSize, X, y)) {
			for(i=0; i<number_output_class; i++) {
				t[i] = 0.0;
				if( (i+1)==(int)y) {
					t[i] = 1.0;
				}
			}
			error += aNet->forward(X, t, 0.01);	
		}
		k++;
	}
	
	error = error / m;
	cout << "###############################################################" << endl;
	cout << "Total error = " << error << endl;
	cout << "###############################################################" << endl << endl;
	
	delete t;
	delete theta1;
	delete theta2;
	delete X;
	delete aNet;
	
	return 0;
}
