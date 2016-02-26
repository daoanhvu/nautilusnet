#include <stdio.h>
#include <stdlib.h>
#include "../bp.h"

/* ========================= Testing purpose ============================================= */
/* ========================================================================== */
long parseLong(char *s)
{
	int i = 0;
	long v = 0;
	while(s[i] != '\0')
	{
		if( (s[i] < 48) || (s[i] > 57) )
			break;

		v = v*10 + (s[i]-48);
		i++;
	}
	return v;
}


double parseDouble(char *s)
{
	long tpn = 1;
	int i = 0, flag=1;
	double tp=0, v = 0;
	while(s[i] != '\0')
	{
		if( s[i] == '.' )
		{
			if(!flag)
			{
				//error
				break;
			}
			flag = 0;
		}
		else
		{
			if( (s[i] < 48) || (s[i] > 57) )
				break;

			if(flag)
				v = v*10 + (s[i]-48);
			else
			{
				tp = tp * 10 + (s[i]-48);
				tpn = tpn * 10;
			}
		}

		i++;
	}
	return v + (tp/tpn);
}

void test_and_function()
{
    int neurals[3];
	long step=0, epochs, iSample=0;
	BPNETWORK *nw = 0;
	double input[4][2] = {{0,0},{0,1},{1,0},{1,1}};
	double target[4][1] = {{0},{0},{0},{1}};
	double e;
	double *biases;
	double threshold = 0.00001;

    nw = (BPNETWORK *)malloc(sizeof(BPNETWORK));
	epochs = 200000;
	nw->learningRate = 0.15;
	nw->alpha = 0.1;

    biases = (double*)malloc(3 * sizeof(double));
	biases[0] = 0.89;
	biases[1] = -0.69;
	biases[2] = 0.55;

	printf("-------------Network configuration-------------\n");
	printf("Epoch: %ld\n", epochs);
	printf("LearningRate: %lf\n", nw->learningRate);
	printf("Alpha(momentum): %lf \n", nw->alpha);
	printf("Threshold: %lf \n", threshold);
	printf("-----------------------------------------------\n");

	if(nw != 0)
	{
		neurals[0] = 3;
		neurals[1] = 3;
		neurals[2] = 1;
		initNetwork(biases, 3, neurals, nw);

		step = 0;
		do{
			inputData(input[iSample], nw);
            e = backPropagate(target[iSample], nw );
			step = step + 1;
			iSample = step % 4;

		}while( (step<epochs) && e>threshold );
		//printf("Mean square error: %lf (minimum error: %lf) (threshold: %lf) after %ld epoch\n", e, minE, threshold, count);
		printf("Training completed with error/threshold %lf/%lf after %ld epochs \n\n", e, threshold, step);

		//Test the network
		printf("This network demonstrate for AND gate: \n");
		inputData(input[0], nw);
		feedForward(nw);
		printf("Output(0,0): %lf\n", nw->layer[nw->nLayer-1].p[0].x);

		inputData(input[1], nw);
		feedForward(nw);
		printf("Output(0,1): %lf\n", nw->layer[nw->nLayer-1].p[0].x);

		inputData(input[2], nw);
		feedForward(nw);
		printf("Output(1,0): %lf\n", nw->layer[nw->nLayer-1].p[0].x);

		inputData(input[3], nw);
		feedForward(nw);
		printf("Output(1,1): %lf\n", nw->layer[nw->nLayer-1].p[0].x);

		release(nw);
		free(nw);
		free(biases);
	}
}

void test_xor_function()
{

}

int main(int argc, char* args[])
{
    test_and_function();
	return 0;
}

