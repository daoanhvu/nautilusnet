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
	layer[0].d = NULL; //Input layer has no delta
	layer[0].a = new Vec<double>(inputSize + 1);
	layer[0].a->setAt(1.0, 0);
	
	//hidden layers
	for(l=1; l<L-1; l++) {
		preSize = layer[l-1].layerSize;
		layer[l].layerSize = hiddenSize;
		//cout << "Layer " << l << "; Hidden size = " << hiddenSize << endl;
		//Number of row of matrix weight is this layerSize
		//Number of column of matrix weight is preSize
		layer[l].weight = new FMat<double>(hiddenSize, preSize + 1); //plus 1 for bias
		layer[l].a = new Vec<double>(hiddenSize + 1);
		layer[l].a->setAt(1.0, 0);
		
		layer[l].d = new Vec<double>(hiddenSize);
	}
	
	//output layer
	preSize = layer[L-2].layerSize;
    //cout << "last preSize = " << preSize << endl;
	layer[L-1].layerSize = outputSize;
	layer[L-1].weight = new FMat<double>(outputSize, preSize + 1); //plus 1 for bias
	layer[L-1].a = new Vec<double>(outputSize);
	layer[L-1].d = new Vec<double>(outputSize);
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
			
			delete layer[l].d;
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
	l->weight->setValues(w);
    //l->weight->print(cout);
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
	double j;
	double tmp;
	
	//Set input data to the input layer
	size = layer[0].layerSize;
	// Start from 1, because we hold bias at 0
	layer[0].a->setValues(x, size, 1);
	
    //This for loop is used for HIDDEN layers ONLY
	for(l=1; l<L-1; l++) {
        //FMat<double> tr = layer[l].weight->transpose();
		//z =  (*(layer[l-1].a)) * tr;
		
		z = mulToTranspose(*(layer[l-1].a), *(layer[l].weight));
		size = z.size();
		for(i=0; i<size; i++) {
			layer[l].a->setAt((1.0/(1.0 + exp(-z[i]))), i+1);
		}
	}
    
    //process for the lass layer (the output layer)
	Layer *last = (Layer*) (layer + (L-1));
    //z =  (*(layer[L-2].a)) * last->weight->transpose();
	z = mulToTranspose(*(layer[L-2].a), *(last->weight));
    size = z.size();
    for(i=0; i<size; i++) {
		last->a->setAt((1.0/(1.0 + exp(-z[i]))), i);
	}
	
	//cout << "hThetaX: " << endl;
    //(last->a)->print(cout);
    
    size = last->layerSize;
	//cout << "last layer size: " << size << endl;
	j = 0.0;
	for(i=0; i<size; i++) {
		tmp = last->a->operator[](i);
		//cout << "output(" << i << ") = " << tmp << "; error = " << (tmp - y[i]) << endl;
		j += -y[i]*log(tmp) - (1.0-y[i]) * log(1.0-tmp);
		
		//Compute delta for ouput layer
		last->d->setAt( tmp - y[i] , i);
	}
	return j;
}

void NautilusNet::backward() {
	int i, size;
	Layer *li;
	
	//l2 is the next layer of li (l(i+1))
	Layer *l2;
	
	for(i=L-2; i>0; i--) {
		li = (Layer*)(layer+i);
		l2 = (Layer*)(layer+i+1);
		
		//li->d = l2->weights->transpose() * (l2->d) .* g'(z(3))
		//computeDelta(i, z);
	}
}

void NautilusNet::computeDelta(int targetIdx, Vec<double> z) {
    Layer *layer = (Layer*) (layer + targetIdx);
    Layer *layer2 = (Layer*) (layer + targetIdx + 1);
    
	int row2 = layer2.row;
    int col2 = layer2->column;
	int l = v.size();
	int i, j;
    double s;
    for(i=0; i<m2.column; i++) {
        s = (T)0;
        for(j=0; j<l; j++) {
            s += v[j] * layer2->weight->operator[](j).operator[](i);
        }
        layer->d->setAt(s, i);
    }
}

double NautilusNet::sigmoid(double z) {
    return (1.0/(1.0 + exp(-z)));
}

double NautilusNet::gradientSigmoid(double z) {
	//g = sigmoid(z) .* (1-sigmoid(z));
	double sgm = 1.0/(1.0 + exp(-z));
    return sgm * (1.0 - sgm);
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