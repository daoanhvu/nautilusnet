#ifndef _NAUTILUSNET_H
#define _NAUTILUSNET_H

#include <cstddef>
#include "fmat.hpp"

using namespace gm;

typedef struct tagLayer {
	//This is number of node in this layer
	int layerSize;
	//This is output value of the previous layer so it's also the input of this layer
	//THIS VECTOR IS INCLUDING BIAS
	FMat<double> *a;
	
	//This vector is use to hold delta, the length of this vector equals to layerSize
	FMat<double> *d;
} Layer;

class NautilusNet {
	private:
		Layer *layer;
		int L;
		
		//This is the weight matrix between the previous layer and this layer
		//Length of this array is L-1
		FMat<double> *weight;
		//This array of matrix is used to hold gradient of weights
		//Length of this array is L-1
		FMat<double> *dw;
		
		double regulator(int m, double lambda);
		void computeDelta(int idx, Layer *l, const Layer *l2, const FMat<double> &z);
		
	public:
		NautilusNet();
		NautilusNet(int layerCount, int inputSize, int hiddenSize, int outputSize);
        ~NautilusNet();
		
		int getL(){
			return L;
		}

		FMat<double> getWeights(int index) {
			return weight[index];
		}

		//for debuging
		FMat<double> getGradients(int index) {
			return dw[index];
		}
		
		/**
			set weights to the net at the middle of layer idx and (idx + 1)
		*/
		void setWeights(int idx, const double *w);
		
		void setInputOutput(const double *inputs, const double* ouputs);
		double forward(int m, const double *x, const double *y, double lambda);
		void backward();
		void updateWeights(int m, double lambda);
        double sigmoid(double);
		double gradientSigmoid(double z);
        double costFunction(const double **x, int m, double *y, double lambda, double *grad, int &gradSize);
		
};

#endif