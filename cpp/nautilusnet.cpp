#include "nautilusnet.h"
#include <cmath>
#include "fmat.hpp"
#ifdef DEBUG
#include <iostream>
using namespace std;
#endif

//using namespace gm;

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
	layer[0].weight = NULL; //Input layer has no weights
	layer[0].a = new double[inputSize];
	
	//hidden layers
	for(l=1; l<L-1; l++) {
		preSize = layer[l-1].layerSize;
		layer[l].layerSize = hiddenSize;
		
		//Number of row of matrix weight is this layerSize
		//Number of column of matrix weight is preSize
		layer[l].weight = new FMat<double>(hiddenSize, preSize);
		layer[l].a = new double[hiddenSize];
	}
	
	//output layer
	preSize = layer[l-2].layerSize;
	layer[L-1].layerSize = outputSize;
	layer[L-1].weight = new FMat<double>(outputSize, preSize);
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
			if(layer[l].weight != NULL) {
				delete layer[l].weight;
			}
			if(layer[l].a != NULL)
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