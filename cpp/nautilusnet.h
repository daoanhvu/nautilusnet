#ifndef _NAUTILUSNET_H
#define _NAUTILUSNET_H

#include <cstddef>
#include "fmat.hpp"

using namespace gm;

typedef struct tagLayer {
	//This is number of node in this layer
	int layerSize;
	
	//This is the weight matrix between the previous layer and this layer
	FMat<double> *weight;
	
	//This is output value of the previous layer so it's also the input of this layer
	//THIS VECTOR IS INCLUDING BIAS
	Vec<double> *a;
} Layer;

class NautilusNet {
	private:
		Layer *layer;
		int L;
		
	public:
		NautilusNet();
		NautilusNet(int layerCount, int inputSize, int hiddenSize, int outputSize);
        ~NautilusNet();
		
		void setL(int l) {
			L = l;
		}
		
		int getL(){
			return L;
		}
		
		/**
			set weights to the net at the middle of layer idx and (idx + 1)
		*/
		void setWeights(int idx, const double *w);
		
		void setInputOutput(const double *inputs, const double* ouputs);
		double forward(const double *x, const double *y, double lambda);
		void backward();
        double sigmoid(double);
        double costFunction(const double **x, int m, double *y, double lambda, double *grad, int &gradSize);
		
};

#endif