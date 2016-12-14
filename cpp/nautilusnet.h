#ifndef _NAUTILUSNET_H
#define _NAUTILUSNET_H

#include <cstddef>

typedef struct tagLayer {
	int layerSize;
	double **weights;
	double *a;
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
		
		void setInputOutput(const double *inputs, const double* ouputs);
		void forward(const double **x, double *y, double lambda);
		void backward();
        double sigmoid(double);
        void costFunction(double **x, double *y, double lambda, double J, double *grad, int &gradSize);
		
};

#endif