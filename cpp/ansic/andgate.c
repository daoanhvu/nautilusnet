#include<stdio.h>
#include "bp.h"

int main(int argc, char* args[])
{
	int i, j, k, ch=1, neurals[3];
	long step=0, count, iSample=0;
	BPNETWORK *nw = 0;
	double input[4][2] = {{0,0},{0,1},{1,0},{1,1}};
	double target[4][1] = {{0},{0},{0},{1}};
	double error[4];
	double e, minE = 1.0;
	double threshold, biases[3], temp;
	
	if(argc < 4)
	{
		printf("[Usage] bp <number of step> <learning rate> <anpha>\n");
		return 1;	
	}
	
	count = parseLong(args[1]);
	threshold = 0.000001;
	nw = (BPNETWORK *)malloc(sizeof(BPNETWORK));
	
	nw->learningRate = parseDouble(args[2]);
	nw->anpha = parseDouble(args[3]);
	
	biases[0] = 0.79;
	biases[1] = 0.599;
	biases[2] = 0.65;
	
	printf("Network status: %ld epochs, learningRate: %lf, anpha: %lf \n", count, nw->learningRate, nw->anpha);
	
	if(nw != 0)
	{
		neurals[0] = 3;
		neurals[1] = 3;
		neurals[2] = 1;
		initNetwork(biases, 3, neurals, nw);
		
		do{		
			step = 0;
			do{
				inputData(input[iSample], nw);
				train(target[iSample], nw );
				
				e = target[iSample][0] - nw->layer[nw->nLayer-1].p[0].x;
				e = e * e;
					
				error[iSample] = e;
				for(i=0;i<=iSample;i++)
					e = e + error[iSample];
				e = e/(iSample + 1);
				
				if(e < minE)
					minE = e;
				step = step + 1;
				
				iSample = iSample + 1;
				if(iSample == 4)
					iSample = 0; 
				
			}while( (step<count) && e>threshold );
			//printf("Mean square error: %lf (minimum error: %lf) (threshold: %lf) after %ld epoch\n", e, minE, threshold, count);
			printf("Training completed with error/threshold %lf/%lf \n", e, threshold);	
			printf("Do you want to update biases and train again? 1 = 'Yes' OR 0='No'\n");
			scanf("%d", &ch);
		}while( ch != 0 );
		
		//Test the network
		printf("This network demonstrate for AND gate: ");
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
	}
	
	return 0;
}
