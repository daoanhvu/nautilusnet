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

/**
	Size of the net excluded biases
*/
NautilusNet::NautilusNet(int layerCount, int inputSize, int hiddenSize, int outputSize) {
	int l, r, preSize;
	L = layerCount;
	layer = new Layer[L];
	
	//input layer
	layer[0].layerSize = inputSize;
	layer[0].weight = NULL; //Input layer has no weights
	layer[0].a = new Vec<double>(inputSize + 1);
	layer[0].a->setAt(1.0, 0);
	
	//hidden layers
	for(l=1; l<L-1; l++) {
		preSize = layer[l-1].layerSize;
		layer[l].layerSize = hiddenSize;
		
		//Number of row of matrix weight is this layerSize
		//Number of column of matrix weight is preSize
		layer[l].weight = new FMat<double>(hiddenSize, preSize + 1); //plus 1 for bias
		layer[l].a = new Vec<double>(hiddenSize + 1);
		layer[l].a->setAt(1.0, 0);
	}
	
	//output layer
	preSize = layer[l-2].layerSize;
	layer[L-1].layerSize = outputSize;
	layer[L-1].weight = new FMat<double>(outputSize, preSize + 1); //plus 1 for bias
	layer[L-1].a = new Vec<double>(outputSize);
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

void NautilusNet::setWeights(int idx, const double *w) {
	Layer *l = (Layer*) (layer + (idx + 1));
	int size = l->layerSize;
	
	layer[idx+1].weight->setValues(w);
}

/**
	This method returns the error value of the forward circle.
	Params:
		x input vertor
		y this is the target vector has size number-of-class that is the 'right' class of input x,
		if this is target k-th then y[k] = 1.0 otherwise y[k] = 0
*/
double NautilusNet::forward(const double *x, const double *y, double lambda) {
	int l, i;
	
	Vec<double> z;
	int size;
	double j = 0;
	double tmp;
	
	//Set input data to the input layer
	// Start from 1, because we hold bias at 0
	layer[0].a->setValues(x, 1);
	
	for(l=1; l<L; l++) {
		z = *(layer[l].weight) * (*(layer[l-1].a));
		size = z.size();
		for(i=0; i<size; i++) {
			layer[l].a->setAt((1.0/(1.0 + exp(z[i]))), i);
		}
	}
	
	Layer *last = (Layer*) (layer + (L-1));
	size = last->layerSize;
	for(i=0; i<size; i++) {
		tmp = last->a->operator[](i);
		//cout << "Temp error: " << tmp << endl;
		j += -y[i]*log(tmp) - (1.0-y[i]) * log(1.0-tmp);
	}
	
	return j;
}

void NautilusNet::backward() {
}

double NautilusNet::sigmoid(double z) {
    return (1.0/(1.0 + exp(z)));
}

/**
	Params:
		m: number of training example
*/
double NautilusNet::costFunction(const double **x, int m, double *y, double lambda, double *grad, int &gradSize) {
	int i, l;
	double jValue = 0;
	
	for(i=0; i<m; i++) {
	}
	
	return jValue;
}