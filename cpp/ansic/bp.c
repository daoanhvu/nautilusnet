#include<stdio.h>
#include<stdlib.h>
#include<math.h>
#include<time.h>
#include "bp.h"

/*
	Author: Dao Anh Vu
	Description: build network structure from data file, read nautilus_bp_spec.pdf for data file structure
	
	Input	: openned FILE pointer, allocated backpropagation network pointer
	Output: 1 if read OK, otherwise return 0.
*/
int loadNetwork(FILE *f, BPNETWORK *nw)
{
	/* read file ID, 2 first bytes, to check if it's a right file structure */
	char fileCode[2];
	int i, j, k, c, numOfLayer, tmp, nWeight=0;
	int *neurals;
	double *weights;

	fread(fileCode, sizeof(char), 2, f);

	if( fileCode[0] != 'B' || fileCode[1] != 'P')
		return 0;

	/* Read number of layer */
	fread(&numOfLayer, sizeof(int), 1, f);
	neurals = (int*)malloc(numOfLayer * sizeof(int));

	fread(neurals, sizeof(int), numOfLayer, f);

	nw->nLayer = numOfLayer;
	nw->layer = (LAYER*)malloc(nw->nLayer * sizeof(LAYER));
	for(i=0; i<nw->nLayer; i++) {
		nw->layer[i].nNeural = neurals[i];
		nw->layer[i].p = (NEURAL*)malloc(nw->layer[i].nNeural * sizeof(NEURAL));

		if( i < (nw->nLayer - 1) ){
			tmp = neurals[i+1];
			if( (i + 1) < (nw->nLayer - 1))
				tmp = neurals[i+1] - 1;

			for(j=0; j<neurals[i]; j++)	{
				nw->layer[i].p[j].w = (double*)malloc(tmp * sizeof(double));
				nw->layer[i].p[j].preDw = (double*)malloc(tmp * sizeof(double));
			}
		} else {
			for(j=0; j<neurals[i]; j++) {
				nw->layer[i].p[j].w = NULL;
				nw->layer[i].p[j].preDw = NULL;
			}
		}
	}

	for(i=0; i<nw->nLayer-1; i++) {
		tmp = neurals[i] * (neurals[i+1]);
		if(i+1 < nw->nLayer-1)
			tmp = neurals[i] * (neurals[i+1]-1);

		nWeight = nWeight + tmp;
	}

	/* Read weights from files */
	weights = (double*)malloc(nWeight * sizeof(double));
	fread(weights, sizeof(double), nWeight, f);
	c=0;
	for(i=0; i<nw->nLayer-1; i++) {
		tmp = neurals[i+1];
		if( (i + 1) < (nw->nLayer - 1))
			tmp = neurals[i+1] - 1;

		for(j=0; j<neurals[i]; j++) {
			for(k=0; k<tmp; k++) {
				nw->layer[i].p[j].w[k] = weights[c];
				c = c + 1;
				//nw->layer[i].p[j].preDw = (double*)malloc(tmp * sizeof(double));
			}
		}
	}

	free(weights);
	free(neurals);
	return 1;
}

/* 
	Initialize parameters for the network.
	Input:
		b: bias array
		nlayer: number of layer in the network
		neurals: number of neural in each layer, this is an array
		nw: our network
 */
void initNetwork( double *b, int nlayer, int *neurals, BPNETWORK *nw ) {
	int i, j, k, temp, iBias = 0;
	srand(time(0));
	nw->nLayer = nlayer;
	nw->layer = (LAYER*)malloc(nw->nLayer * sizeof(LAYER));

	for(i=0; i<nw->nLayer; i++)	{
		nw->layer[i].nNeural = neurals[i];
		nw->layer[i].p = (NEURAL*)malloc(nw->layer[i].nNeural * sizeof(NEURAL));
	}

	/* Memory allocate for weights */
	for(i=0;i<nw->nLayer - 1 ;i++)
		for(j=0;j<nw->layer[i].nNeural;j++)	{
			/* By default, we think that the next layer is a hidden layer */
			temp = nw->layer[i+1].nNeural - 1;
			/* if the next layer is the output layer */
			if( i + 1 == nw->nLayer-1 )	{
				temp = nw->layer[i+1].nNeural;
			}

			//printf("Dang cap phat %d weight cho neural %d tai layer %d\n", temp, j, i);

			nw->layer[i].p[j].w = (double*)malloc( temp * sizeof(double) );
			nw->layer[i].p[j].preDw = (double*)malloc( temp * sizeof(double) );
			//printf("Cap layer:%d, neural:%d, num of weight:%d \n ", i, j, nw->layer[i+1].nNeural);
			for(k=0; k<temp; k++)
			{
				if( j == nw->layer[i].nNeural-1)
				{
					nw->layer[i].p[j].w[k] = b[iBias];
					iBias = iBias + 1;
				}
				else
					nw->layer[i].p[j].w[k] = (rand()/((double)RAND_MAX + 1))/2;

				nw->layer[i].p[j].preDw[k] = 0.0;
				//printf("   Layer(%d), neural(%d), w(%d) = %lf\n", i, j, k, nw->layer[i].p[j].w[k]);
			}
		}

	//
	for(i=0;i<nw->layer[nw->nLayer - 1].nNeural ;i++)
	{
		nw->layer[nw->nLayer - 1].p[i].w = NULL;
		nw->layer[nw->nLayer - 1].p[i].preDw = NULL;
	}
}

void inputData(double *input, BPNETWORK *nw)
{
	int i;
	/* Feed to input layer */
	for(i=0; i<nw->layer[0].nNeural - 1 ; i++)
		nw->layer[0].p[i].x = input[i];

	/* set input for bias = 1 */
	nw->layer[0].p[nw->layer[0].nNeural - 1].x = 1;
}

void updateBiases(double *b, BPNETWORK *nw)
{
	int i, k, temp, ib=0;
	for(i=0;i<nw->nLayer - 1 ;i++)
	{
		temp = nw->layer[i+1].nNeural - 1;
		/* if the next layer is the output layer */
		if( i + 1 == nw->nLayer-1 )
			temp = nw->layer[i+1].nNeural;

		for(k=0; k<temp; k++)
		{
			nw->layer[i].p[nw->layer[i].nNeural-1].w[k] = b[ib];
			ib = ib + 1;
		}
	}
}

void feedForward(BPNETWORK *nw)
{
	int i, j, k, n;
	double sum = 0;

	/* Calculate the input for neural j at layer i except bias neural */
	for(i=1;i<nw->nLayer; i++)
	{
		n = nw->layer[i].nNeural - 1;
		if(i == nw->nLayer-1)
			n = n + 1;

		for(j=0; j < n; j++)
		{
			sum = 0;
			for(k=0; k<nw->layer[i-1].nNeural; k++)
			{
				//printf("So layer: %d, neural: %d, weight %lf \n ", i-1,j,nw->layer[i-1].p[k].w[j]);
				sum = sum + (nw->layer[i-1].p[k].x * nw->layer[i-1].p[k].w[j]);
			}
			nw->layer[i].p[j].x = sigmoid(sum);
		}
	}
}

double backPropagate(double *tg, BPNETWORK *nw )
{
	int i, j, k;
	double sumError = 0, error, dw, sum = 0;

	feedForward(nw);

	/* Find the delta for output layer */
	for(i=0;i<nw->layer[nw->nLayer-1].nNeural;i++)
	{
		error = tg[i] - nw->layer[nw->nLayer-1].p[i].x;
		/*     delta = f'(x) * e =  [x*(1-x)] * (target - output)     */
		nw->layer[nw->nLayer-1].p[i].delta = nw->layer[nw->nLayer-1].p[i].x * (1 - nw->layer[nw->nLayer-1].p[i].x) * error;
		sumError = sumError + (error * error);
	}

	/*	Find the delta for the hidden layers */
	for(i=nw->nLayer-2;i>=0;i--)
	{
		/*For each layer, we browse all neural there*/
		for(j=0;j<nw->layer[i].nNeural;j++)
		{
			sum = 0;
			/* Calculate sum delta of precide layer*/
			for(k=0;k<nw->layer[i+1].nNeural;k++)
				sum = sum + nw->layer[i+1].p[k].delta * nw->layer[i].p[j].w[k];

			nw->layer[i].p[j].delta = nw->layer[i].p[j].x * (1 - nw->layer[i].p[j].x) * sum;
		}
	}

	// Update weights
	int temp;
	for(i=0; i<nw->nLayer-1; i++)
	{
		temp = nw->layer[i+1].nNeural - 1;
		if(i+1 == nw->nLayer-1)
			temp = nw->layer[i+1].nNeural;

		for(j=0;j<nw->layer[i].nNeural;j++)
		{
			for(k=0; k<temp; k++)
			{
				// Apply momentum
				dw = (nw->alpha * nw->layer[i].p[j].preDw[k]) + nw->learningRate * nw->layer[i+1].p[k].delta * nw->layer[i].p[j].x;
				nw->layer[i].p[j].w[k] = nw->layer[i].p[j].w[k] + dw;
				nw->layer[i].p[j].preDw[k] = dw;
			}
		}
	}

	return sumError/2.0f;
}

double meanSquareError(double *tgt, double *o, int n)
{
	int i;
	double s = 0;
	for(i=0;i<n;i++)
		s = s + ( tgt[i] - o[i] ) * ( tgt[i] - o[i] );
	return s/2;
}

double sigmoid(double x)
{
	return (1.0/(1 + exp(-x)));
}

void release( BPNETWORK *nw )
{
	int i, j;
	if( nw != NULL )
	{
		if(nw->layer != NULL)
		{
			for(i=0;i<nw->nLayer;i++)
			{
				if(nw->layer[i].p != NULL)
				{
					for(j=0; j< nw->layer[i].nNeural; j++)
					{
						if(nw->layer[i].p[j].w != NULL)
							free(nw->layer[i].p[j].w);

						if(nw->layer[i].p[j].preDw != 0)
							free(nw->layer[i].p[j].preDw);
					}
					free(nw->layer[i].p);
				}
			}
			free(nw->layer);
		}
	}
}
