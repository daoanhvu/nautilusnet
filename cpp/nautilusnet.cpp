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
	weight = NULL;
	dw = NULL;
}

/**
	Size of the net excluded biases
*/
NautilusNet::NautilusNet(int layerCount, int inputSize, int hiddenSize, int outputSize) {
	int l, r, preSize;
	L = layerCount;
	layer = new Layer[L];

	//delta matrices fo weights
	weight = new FMat<double>[L-1];
	dw = new FMat<double>[L-1];

	//input layer
	layer[0].layerSize = inputSize;
	layer[0].d = NULL; //Input layer has no delta
	layer[0].a = new FMat<double>(1, inputSize + 1);
	layer[0].a->setAt(0, 0, 1.0);

	//hidden layers
	for(l=1; l<L-1; l++) {
		preSize = layer[l-1].layerSize;
		layer[l].layerSize = hiddenSize;
		//cout << "Layer " << l << "; Hidden size = " << hiddenSize << endl;
		layer[l].a = new FMat<double>(1, hiddenSize + 1);
		layer[l].a->setAt(0, 0, 1.0);
		layer[l].d = new FMat<double>(1, hiddenSize);

		//Number of row of matrix weight is this layerSize
		//Number of column of matrix weight is preSize
		weight[l-1].init(hiddenSize, preSize + 1);
		//Initialize matrices delta of weights
		dw[l-1].init(hiddenSize, preSize + 1, 0);
	}

	//output layer
	preSize = layer[L-2].layerSize;
    //cout << "last preSize = " << preSize << endl;
	layer[L-1].layerSize = outputSize;
	layer[L-1].a = new FMat<double>(1, outputSize);
	layer[L-1].d = new FMat<double>(1, outputSize);

	weight[L-2].init(outputSize, preSize + 1);
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
			if(layer[l].a != NULL)
				delete layer[l].a;

			delete layer[l].d;

			if(l<L-1) {
				weight[l].release();
				dw[l].release();
			}
		}
		delete[] layer;
		delete[] weight;
		delete[] dw;
		L = 0;
	}
}

void NautilusNet::setInputOutput(const double *inputs, const double* ouputs) {
}

void NautilusNet::setWeights(int idx, const double *w) {
	weight[idx].setValues(w);
}

/**
	This method returns the error value of the forward circle.
	Params:
		m int the total number of training example
		x input vertor
		y this is the target vector has size number-of-class that is the 'right' class of input x,
		if this is target k-th then y[k] = 1.0 otherwise y[k] = 0
*/
double NautilusNet::forward(int m, const double *x, const double *y, double lambda) {
	int l, i;

	FMat<double> z;
	int size;
	double j;
	double hThetaX;

	//Set input data to the input layer
	size = layer[0].layerSize;
	// Start from 1, because we hold bias at 0
	layer[0].a->setValues(x, 0, size, 1);

    //This for loop is used for HIDDEN layers ONLY
	for(l=1; l<L-1; l++) {
		z = layer[l-1].a->mulToTranspose(weight[l-1]);
		size = z.getColumn();
		for(i=0; i<size; i++) {
			layer[l].a->setAt(0, i+1, (1.0/(1.0 + exp(-z.value(0, i)))));
		}
	}

    //process for the lass layer (the output layer)
	Layer *last = (Layer*) (layer + (L-1));
    //z =  (*(layer[L-2].a)) * last->weight->transpose();
	z = layer[L-2].a->mulToTranspose(weight[L-2]);
	//z.print(cout);

    size = z.getColumn();
	j = 0.0;
    for(i=0; i<size; i++) {
		hThetaX = 1.0/(1.0 + exp(-z.value(0,i)));
		last->a->setAt(0, i, hThetaX);
		j += -y[i]*log(hThetaX) - (1.0-y[i]) * log(1.0-hThetaX);
		//Compute delta for ouput layer
		last->d->setAt(0, i, hThetaX - y[i]);
        //cout << "a3: "<< hThetaX << " d3: " << hThetaX - y[i] << endl;
	}

	//double rg = regulator(1, lambda);
	//double rg = regulator(m, lambda);
	//cout << "\nregulator: " << rg << endl;

	return (j + regulator(m, lambda));
}

/**
	This backward is computing deltas just for hidden layers only,
	delta values for the last layer is already computed in forward step

	Call this  routine after each training iteration
*/
void NautilusNet::backward() {
	int i, size;
	Layer *l0;
	Layer *li;
	//l2 is the next layer of li (l(i+1))
	Layer *l2;
	FMat<double> z;

	for(i=L-2; i>0; i--) {
		l0 = (Layer*)(layer+i-1);
		li = (Layer*)(layer+i);
		l2 = (Layer*)(layer+i+1);
		//z = (*w) * (*(l0->a)');
		z = weight[i-1].mulToTranspose(*(l0->a));

		//li->d = l2->weights->transpose() * (l2->d) .* g'(z(3))
		computeDelta(i, li, l2, z);
	}

	//Compute gradients
	for(i=0; i<L-1; i++) {
		li = (Layer*)(layer+i);
		l2 = (Layer*)(layer+i+1);

		dw[i] += l2->d->mulTransposeTo(*(li->a));

		//Reset d matrix
		l2->d->setZero();
	}


}
/**
	TODO: this is not correct, please fix it as soon as possible;
	Update weights
*/
void NautilusNet::updateWeights(int m, double lambda) {
	int i;
	for(i=0; i<L-1; i++) {
		//cout << weight[i].replaceColumnBy(0, 0.0) << endl;
		dw[i] = (1.0/m) * dw[i] + (lambda/m) * weight[i].replaceColumnBy(0, 0.0);
		weight[i] = weight[i] - dw[i];

		dw[i].setZero();
	}
}

double NautilusNet::regulator(int m, double lambda) {
	int i, c, r, row, column;
	double s = 0.0;
	double tmp = 0.0;

	for(i=0; i<L-1; i++) {
		row = weight[i].getRow();
		column = weight[i].getColumn();
		for(r=0; r<row; r++) {
			//for(c=0; c<column; c++) {
			//We don't include bias's weight into regulator
			for(c=1; c<column; c++) {
				tmp = weight[i][r][c];
				s += (tmp * tmp);
			}
		}
	}
	//cout << "\nSum theta: " << s << endl << (lambda/(2.0*m)) << endl;
	return (lambda/(2.0*m)) * s;
}

void NautilusNet::computeDelta(int idx, Layer *l, const Layer *l2, const FMat<double> &z) {
	int row2;
	int i;
    FMat<double> dt = weight[idx].mulTransposeTo(l2->d->transpose());

	//z.printSize(cout) << " " << col2 << endl;
	//l->d->printSize(cout);
	/**
		Here, we get transpose of weight then multiply to delta
	*/

    // if(idx==1)
    // 	cout << "d3:" << endl << *(l2->d);
		//cout << "mulTransposeTo m:" << endl << dt;
	// weight[idx].printSize(cout);
	// cout << endl;

	// cout << " l2->d: " ;
	// l2->d->printSize(cout);
	// cout << endl;

	// cout << "z: " ;
	// z.printSize(cout);
	// cout << endl;

	// cout << "l->d: ";
	// l->d->printSize(cout);
	// cout << endl;

	row2 = dt.getRow();
	//col2 = dt.getColumn();
	for(i=1; i<row2; i++) {
		l->d->setAt(0, i-1, dt.value(i, 0) * gradientSigmoid(z.value(i-1, 0)));
	}
}

double NautilusNet::sigmoid(double z) {
    return (1.0/(1.0 + exp(-z)));
}

FMat<double> NautilusNet::gradientSigmoidM(FMat<double> z) {
	int row = z.getRow();
	int column = z.getColumn();
	FMat<double> r(row, column);

	for(int i=0; i<row; i++) {
		for(int j=0; j<column; j++) {
			r[i][j] = gradientSigmoid(z.value(i,j));
		}
	}

	return r;
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

void NautilusNet::conv2d(float *input, int rows, int cols, float *filter, int filter_rows, int filter_cols) {

}
