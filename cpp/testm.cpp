#include <iostream>
#include <fstream>
#include <string>
#include <stdlib.h>
#include <cmath>
#include <cfloat>
#include "fmat.hpp"

using namespace gm;
using namespace std;

void testMatrix() {
	double vs[3] = {3.6, 1.2, 2.0};
	double ms[6] = {5.2, 2.0, 1.4, 0.7, 4.1, 0.5};
	FMat<double> m(ms, 2, 3);
	FMat<double> v(vs, 3, 1);

	cout << m << endl;
	cout << v << endl;
	
	//Vec<double> t = m * v;
	FMat<double> t;
	t = m * v;
	
	cout << t << endl;
}

/**
	Test multiply transpose matrix
*/
void testMultiplyTransposeMatrix() {
	double vs[6] = {1.0, 0.7773, 0.80139, 0.77277, 0.84839, 0.84436};
	double ms[12] = {	0.23, 0.18, 0.40, 0.74, 0.12, 0.35,
						0.66, 0.27, 0.54, 0.40, 0.21, 0.5};
	FMat<double> m(ms, 2, 6);
	FMat<double> v(vs, 1, 6);

	cout << m << endl;
	cout << v << endl;
	
	FMat<double> t;
	t = v * m.transpose();
	
    cout << "M transpose: " << endl;
    cout << m.transpose() << endl;
    cout << "Result: " << endl;
	cout << t << endl;
}

void testReadFile(int argc, char **args) {
	ifstream f("/cygdrive/d/data/coursera_X.data", ios::binary);
	
	int index = (int)strtol(args[1], NULL, 10);
	
	int m, i, j;
	int ftSize;
	double tmp;
	double X[400];
	
	f.read((char*)&m, sizeof(int));
	cout << "Number of examples: " << m << endl;
	f.read((char*)&ftSize, sizeof(int));
	cout << "Number of features: " << ftSize << endl;
	
	for(i=0; i<1; i++) {
		for(j=0; j<ftSize; j++) {
			//f.read((char*)&tmp, sizeof(double));
			//X[j] = tmp;
			f.read((char*)&X[j], sizeof(double));
			//cout << "X[" << j <<"]: " << X[j] << endl;
		}
	}
	
	cout << "X["<< index <<"] = " << X[index] << endl;
	
	f.close();
}


void calculateAccuracy(int nSample, int nRotation, int nClassLabel, const float* bottom_label, const float* bottom_data) {
	float tmp_prob;
	float maxval;
  	float maxval_gt;
  	float max_prob;

  	const float kLOG_THRESHOLD = 1e-20;
  		
	int i, j, c, idx;
	int max_id, max_ang;
	const int dim = nRotation * nClassLabel;
	float accuracy = 0.0f;
	float logprob = 0.0f;

	float *scores = new float[nClassLabel-1];

	for(int n=0; n<nSample; n++) {
		maxval = -FLT_MAX;
    	maxval_gt = -FLT_MAX;
    	max_id = 0;
    	max_ang = 0;
    	max_prob = -FLT_MAX;
		for(i=0; i<nRotation; ++i) {
	    	for(c = 0; c < nClassLabel-1; ++c) {
	    		tmp_prob = 0;
	    		for (j = 0; j < nRotation; ++j) {
		   			idx = i + j;
		   			if( idx > nRotation - 1 ) idx -= nRotation;
		   			tmp_prob += log( bottom_data[ ( n * nRotation + idx ) * dim + j * nClassLabel + c ] ) - 
						log( max(bottom_data[ ( n * nRotation + idx ) * dim + j * nClassLabel + nClassLabel - 1 ], FLT_MIN) );
				}
		       	if( tmp_prob > max_prob ){
		           	max_ang = i;
		           	max_prob = tmp_prob;
		       	}
	    	}

	    	// cout << endl << "max prob: " << max_prob << endl;
	    }
	    // calc scores ==========
	    for (int j = 0; j < nClassLabel - 1; ++j)
	    	scores[ j ] = 0;
	    	
	    for (int i = 0; i < nRotation; ++i) {
	    	int idx = i + max_ang;
	    	
			if( idx > nRotation - 1 )
		  		idx -= nRotation;

	      	for (int j = 0; j < nClassLabel - 1; ++j)
				scores[ j ] += log( bottom_data[ ( n * nRotation + idx ) * dim + i * nClassLabel + j ] ) - log( max(bottom_data[ ( n * nRotation + idx ) * dim + i * nClassLabel + nClassLabel - 1 ], FLT_MIN) );
	    }

	    // calc accuracy
	    for (int j = 0; j < nClassLabel - 1; ++j) {
	    	if (scores[ j ] > maxval) {
				maxval = scores[ j ];
				max_id = j;
	    	}
	    	if( j == static_cast<int>(bottom_label[ n * nRotation ]) )
				if (scores[ j ] > maxval_gt)
					maxval_gt = scores[ j ];
	    }

	    if (max_id == static_cast<int>(bottom_label[ n * nRotation ])) {
	     	++accuracy;
	    }
	    float prob = max(maxval_gt, kLOG_THRESHOLD);
	    logprob -= log(prob);
	}

	cout << "Accuracy: " << accuracy / nSample << endl;
	cout << "Logprob: " << logprob / nSample << endl;

	delete[] scores;
}

void mySoftmax(const float *x, int count, float *output) {
	float *exps = new float[count];
	float sum_exp = 0;
	float max_x = x[0];
	float shiftx;

	for(int i=1; i<count; i++) {
		if(x[i] > max_x)
			max_x = x[i];
	}

	for(int i=0; i<count; i++) {
		//Normalize before calc exp
		shiftx = x[i] - max_x;
		exps[i] = exp(shiftx);
		sum_exp += exps[i];
	}

	// cout << endl << "SumExp: " << sum_exp << endl;

	for(int i=0; i<count; i++) {
		output[i] = exps[i] / sum_exp;
	}

	delete[] exps;
}

void testSoftmax() {
	float *input;
	float *prob;
	float *my_prob;
	float *label;
	int label_count;
	int bottom_num, bottom_channel;
	int prob_num, prob_channel;
	int k, dim;
	ifstream f;

	int nClassLabel;
	int nSample;
	int nRotation;

	f.open("/home/davu/data/softmax_loss.dat", ios::binary);

	f.read((char*)&nClassLabel, sizeof(int));
	f.read((char*)&nRotation, sizeof(int));
	f.read((char*)&nSample, sizeof(int));

	cout << "nRotation: " << nRotation << endl;
	cout << "nClassLabel: " << nClassLabel << endl;
	cout << "nSample: " << nSample << endl;

	dim = nClassLabel * nRotation;
	int num = nSample * nRotation;

	int bottom_count = nSample * nRotation * dim;
	input = new float[bottom_count];
	cout << "Bottom_count: " << bottom_count << endl;
	for(int n=0; n<nSample; n++) {
		for(int i=0; i<nRotation; i++) {
			for(int j=0; j<dim; j++) {
				f.read((char*)&input[n*nRotation*dim + j], sizeof(float));
				cout << input[n*nRotation*dim + j] << " ";
			}
			cout << endl;
		}
		cout << endl;
	}

	cout << endl << "Softmax probability:" << endl;
	prob = new float[bottom_count];
	for(int n=0; n<nSample; n++) {
		for(int i=0; i<nRotation; i++) {
			for(int j=0; j<dim; j++) {
				f.read((char*)&prob[n*nRotation*dim + j], sizeof(float));
				cout << prob[n*nRotation*dim + j] << " ";
			}
			cout << endl;
		}
		cout << endl;
	}
	cout << endl;

	cout << endl << "Label count:" << num << endl;
	label = new float[num];
	for(int i=0; i<num; i++) {
		f.read((char*)&label[i], sizeof(float));
		cout << label[i] << " ";
	}
	cout << endl;

	cout << endl << "max_ang:" << endl;
	int *max_ang = new int[nSample];
	for(int i=0; i<nSample; i++) {
		f.read((char*)&max_ang[i], sizeof(float));
		cout << max_ang[i] << " ";
	}
	cout << endl;	

	f.close();

	//Calculate my_pro
	// cout << endl << "My softmax:" << endl;
	// cout << endl;
	// my_prob = new float[bottom_count];
	// for(int i=0; i<prob_num; i++) {
	// 	mySoftmax(input+(i*prob_channel), prob_channel, my_prob+(i*prob_channel));
	// 	for(int j=0; j<prob_channel; j++) {
	// 		k = i * prob_channel + j;
	// 		cout << my_prob[k] << " ";
	// 	}
	// 	cout << endl;
	// }
	// cout << endl;


	// calculateAccuracy(nSample, nRotation, nClassLabel, label, prob);

	delete[] input;
	delete[] prob;
	delete[] label;
	// delete[] my_prob;
}



int main(int argc, char **args) {
	
	//testReadFile(argc, args);
    //testMatrix();
	// testMultiplyTransposeMatrix();
	testSoftmax();
	
	return 0;
}