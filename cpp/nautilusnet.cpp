#include "nautilusnet.h"
#include <cmath>

#ifdef DEBUG
#include <iostream>
using namespace std;
#endif

NautilusNet::NautilusNet() {
	L = 0;
	layer = NULL;
}

NautilusNet::NautilusNet(int layerCount, int inputSize, int hiddenSize, int outputSize) {
	int l, r, preSize;
	L = layerCount;
	layer = new Layer[L];
	
	//input layer
	layer[0].layerSize = inputSize;
	layer[0].weights = NULL; //Input layer has no weights
	layer[0].a = new double[inputSize];
	
	//hidden layers
	for(l=1; l<L-1; l++) {
		preSize = layer[l-1].layerSize;
		layer[l].layerSize = hiddenSize;
		
		//Number of row of matrix weight is this layerSize
		layer[l].weights = (double **) new double*[hiddenSize];
		//Number of column of matrix weight is preSize
		for(r=0; r<hiddenSize; r++) {
			layer[l].weights[r] = new double[preSize];
		}
		
		layer[l].a = new double[hiddenSize];
	}
	
	//output layer
	preSize = layer[l-2].layerSize;
	layer[L-1].layerSize = outputSize;
	layer[L-1].weights = (double **) new double*[outputSize];
	for(r=0; r<outputSize; r++) {
		layer[l].weights[r] = new double[preSize];
	}
	layer[L-1].a = new double[outputSize];
}

NautilusNet::~NautilusNet() {
	int l, ls;
	#ifdef DEBUG
		cout << "Going to release the Net" << endl;
	#endif
    if(L > 0) {
		for(l=0; l<L; l++) {
			ls = layer[l].layerSize;
			#ifdef DEBUG
				cout << "Release layer " << l << " with " << ls << " node(s) " << endl;
			#endif
			if(layer[l].weights != NULL) {
				for(int i=0; i<ls; i++) {
					delete layer[l].weights[i];
				}
				delete layer[l].weights;
			}
			delete layer[l].a;
		}
		delete layer;
		L = 0;
	}
}
		
void NautilusNet::setInputOutput(const double *inputs, const double* ouputs) {
}

void NautilusNet::forward(const double **x, double *y, double lambda) {
	int l;
	
	for(l=0; l<L; l++) {
		
	}
}

void NautilusNet::backward() {
}

double NautilusNet::sigmoid(double z) {
    return (1.0/(1.0 + exp(z)));
}

void NautilusNet::costFunction(double **x, double *y, double lambda, double J, double *grad, int &gradSize) {
	int l;
	
	for(l=0; l<L; l++) {
		if(l > 0) {
			
		}
	}
}