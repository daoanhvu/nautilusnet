/*
	Author: Dao Anh Vu
	This is a kind of Neural Network structure :)
*/
#ifndef _BP_H_
#define _BP_H_

#include<stdio.h>

struct tagInput
{
	double x;
	double *w;
	double *preDw; // to calculate momentum
	double delta;
};
typedef struct tagInput NEURAL;

struct tagLayer
{
	/* here I alway use the last neural as bias of the layer */
	int nNeural;
	NEURAL *p;
};
typedef struct tagLayer LAYER;

struct tagNetwork
{
	double totalError;
	double learningRate, alpha;
	int nLayer;
	LAYER *layer;
};
typedef struct tagNetwork BPNETWORK;

int loadNetwork(FILE *f, BPNETWORK *nw);
void writeNetwork(FILE *f, BPNETWORK *nw);
void initNetwork( double *b, int nlayer, int *neurals, BPNETWORK *nw );
void inputData(double *input, BPNETWORK *nw);
void updateBiases(double *b, BPNETWORK *nw);
void feedForward(BPNETWORK *nw);
double backPropagate( double *tg, BPNETWORK *nw  );
double meanSquareError(double *tgt, double *o, int n);
double sigmoid(double);
void release( BPNETWORK * );


#endif
