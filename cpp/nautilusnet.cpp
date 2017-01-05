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
	
	//delta matrices fo weights
	dw = new FMat<double>[L-1];
	
	//input layer
	layer[0].layerSize = inputSize;
	layer[0].weight = NULL; //Input layer has no weights
	layer[0].d = NULL; //Input layer has no delta
	layer[0].a = new FMat<double>(1, inputSize + 1);
	layer[0].a->setAt(0, 0, 1.0);
	
	//hidden layers
	for(l=1; l<L-1; l++) {
		preSize = layer[l-1].layerSize;
		layer[l].layerSize = hiddenSize;
		//cout << "Layer " << l << "; Hidden size = " << hiddenSize << endl;
		//Number of row of matrix weight is this layerSize
		//Number of column of matrix weight is preSize
		layer[l].weight = new FMat<double>(hiddenSize, preSize + 1); //plus 1 for bias
		layer[l].a = new FMat<double>(1, hiddenSize + 1);
		layer[l].a->setAt(0, 0, 1.0);
		layer[l].d = new FMat<double>(1, hiddenSize);
		
		//Initialize matrices delta of weights
		dw[l-1].init(hiddenSize, preSize + 1, 0);
	}
	
	//output layer
	preSize = layer[L-2].layerSize;
    //cout << "last preSize = " << preSize << endl;
	layer[L-1].layerSize = outputSize;
	layer[L-1].weight = new FMat<double>(outputSize, preSize + 1); //plus 1 for bias
	layer[L-1].a = new FMat<double>(1, outputSize);
	layer[L-1].d = new FMat<double>(1, outputSize);
	
	dw[L-2].init(outputSize, preSize + 1, 0);
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
			
			if(l<L-1) {
				dw[l].release();
			}
		}
		delete layer;
		delete dw;
		L = 0;
	}
}
		
void NautilusNet::setInputOutput(const double *inputs, const double* ouputs) {
}

void NautilusNet::setWeights(int idx, const double *w) {
	Layer *l = (Layer*) (layer + (idx + 1));
	int size = l->layerSize;
	l->weight->setValues(w);
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
	
	FMat<double> z;
	int size;
	double j;
	double tmp;
	
	//Set input data to the input layer
	size = layer[0].layerSize;
	// Start from 1, because we hold bias at 0
	layer[0].a->setValues(x, 0, size, 1);
	
    //This for loop is used for HIDDEN layers ONLY
	for(l=1; l<L-1; l++) {
        //FMat<double> tr = layer[l].weight->transpose();
		//z =  (*(layer[l-1].a)) * tr;
		
		z = layer[l-1].a->mulToTranspose(*(layer[l].weight));
		size = z.getColumn();
		for(i=0; i<size; i++) {
			layer[l].a->setAt(0, i+1, (1.0/(1.0 + exp(-z.value(0, i)))));
		}
	}
    
    //process for the lass layer (the output layer)
	Layer *last = (Layer*) (layer + (L-1));
    //z =  (*(layer[L-2].a)) * last->weight->transpose();
	z = layer[L-2].a->mulToTranspose(*(last->weight));
    size = z.getColumn();
    for(i=0; i<size; i++) {
		last->a->setAt(0, i, (1.0/(1.0 + exp(-z.value(0,i)))));
	}
	
	//cout << "hThetaX: " << endl;
    //(last->a)->print(cout);
    
    size = last->layerSize;
	//cout << "last layer size: " << size << endl;
	j = 0.0;
	for(i=0; i<size; i++) {
		tmp = last->a->operator[](0)[i];
		//cout << "output(" << i << ") = " << tmp << "; error = " << (tmp - y[i]) << endl;
		j += -y[i]*log(tmp) - (1.0-y[i]) * log(1.0-tmp);
		
		//Compute delta for ouput layer
		last->d->setAt(0, i, tmp - y[i]);
	}
	return j;
}

/**
	This backward is computing deltas just for hidden layers only,
	delta values for the last layer is already computed in forward step
*/
void NautilusNet::backward() {
	int i, size;
	Layer *l0;
	Layer *li;
	//l2 is the next layer of li (l(i+1))
	Layer *l2;
	FMat<double> z;
	FMat<double> *w;
	
	for(i=L-2; i>0; i--) {
		l0 = (Layer*)(layer+i-1);
		li = (Layer*)(layer+i);
		l2 = (Layer*)(layer+i+1);
		w = li->weight;
		//z = (*w) * (*(l0->a)');
		z = w->mulToTranspose(*(l0->a));
		
		//w->printSize(cout) << endl;
		//l0->a->printSize(cout) << endl;
		
		//li->d = l2->weights->transpose() * (l2->d) .* g'(z(3))
		//z.print(cout);
		computeDelta(li, l2, z);
	}
	
	//Compute gradients and update weights
	for(i=0; i<L-1; i++) {
		li = (Layer*)(layer+i);
		l2 = (Layer*)(layer+i+1);
		dw[i] += l2->d->transpose() * (*(li->a));
		*(l2->weight) += dw[i];
	}
}

void NautilusNet::computeDelta(Layer *l, const Layer *l2, const FMat<double> &z) {
	int row2 = l2->weight->getRow();
    int col2 = l2->weight->getColumn();
	int lSize = l2->d->getColumn();
	int i, j;
    double s;
	
	//z.printSize(cout) << " " << col2 << endl;
	//l->d->printSize(cout);
	/**
		Here, we get transpose of weight then multiply to delta
	*/
    for(i=0; i<col2; i++) {
        s = 0.0;
        for(j=0; j<lSize; j++) {
            s += l2->d->value(0,j) * l2->weight->value(j, i);
        }
		if(i>0) {
			s = s * gradientSigmoid(z.value(i-1, 0));
			l->d->setAt(0, i-1, s);
		}
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