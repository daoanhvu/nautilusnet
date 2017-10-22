#include <iostream>
#include <fstream>
<<<<<<< HEAD
#include <string>
#include <stdlib.h>

using namespace gm;
=======
#include <string.h>
#include <cmath>
#include <cfloat>

>>>>>>> 79d708da567fab419a90a86dcb182989fd10c71c
using namespace std;

/**
	Test multiply transpose matrix
*/
<<<<<<< HEAD
void softmax(const float *input, float *p, int len) {
	
=======
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
>>>>>>> 79d708da567fab419a90a86dcb182989fd10c71c
}

void softmax_with_loss(const float *input, float *p, int len, float &loss) {

}

// Hard code to get fake data
float *create_p(int &len) {
	float v[] = {
<<<<<<< HEAD
		0.39f, 0.55f, 0.72f, 0.10f, 0.05f,	0.11f, 0.44f, 0.80f, 0.05, 0.09f,	0.35f, 0.21f, 0.63f, 0.19f, 0.17f,
		0.49f, 0.25f, 0.52f, 0.12f,	0.11f,	0.55f, 0.20f, 0.71f, 0.15, 0.15f,	0.45f, 0.24f, 0.53f, 0.21f, 0.19f,
		0.14f, 0.45f, 0.63f, 0.20f,	0.14f,	0.19f, 0.42f, 0.68f, 0.17, 0.14f,	0.29f, 0.29f, 0.58f, 0.15f, 0.02f,

		0.26f, 0.65f, 0.52f, 0.31f, 0.22f,	0.09f, 0.45f, 0.28f, 0.18, 0.17f,	0.40f, 0.55f, 0.50f, 0.42f, 0.35f,
		0.44f, 0.81f, 0.47f, 0.37f,	0.31f,	0.30f, 0.60f, 0.60f, 0.27, 0.20f,	0.19f, 0.61f, 0.22f, 0.39f, 0.18f,
		0.38f, 0.73f, 0.41f, 0.15f,	0.14f,	0.51f, 0.84f, 0.66f, 0.45, 0.32f,	0.31f, 0.43f, 0.13f, 0.40f, 0.21f
=======
		0.39f, 0.55f, 0.62f, 0.10f, 0.05f,	0.11f, 0.44f, 0.85f, 0.05, 0.09f,	0.35f, 0.21f, 0.63f, 0.19f, 0.17f,
		0.49f, 0.25f, 0.52f, 0.12f,	0.11f,	0.55f, 0.20f, 0.71f, 0.15, 0.15f,	0.45f, 0.24f, 0.79f, 0.21f, 0.05f,
		0.14f, 0.45f, 0.93f, 0.10f,	0.14f,	0.19f, 0.42f, 0.68f, 0.17, 0.14f,	0.29f, 0.29f, 0.58f, 0.15f, 0.12f,

		0.26f, 0.65f, 0.52f, 0.31f, 0.22f,	0.09f, 0.45f, 0.28f, 0.18, 0.17f,	0.40f, 0.55f, 0.50f, 0.42f, 0.15f,
		0.44f, 0.81f, 0.47f, 0.37f,	0.01f,	0.31f, 0.62f, 0.61f, 0.27, 0.20f,	0.19f, 0.61f, 0.22f, 0.39f, 0.18f,
		0.38f, 0.73f, 0.41f, 0.15f,	0.04f,	0.51f, 0.84f, 0.66f, 0.45, 0.22f,	0.31f, 0.43f, 0.13f, 0.40f, 0.21f
>>>>>>> 79d708da567fab419a90a86dcb182989fd10c71c
	};

	len = 90;
	float *p = new float[len];
	memcpy(p, v, sizeof(float) * len);

	return p;
}

int main(int argc, char **args) {
	
	int nRotation = 3;
	int nPose = nRotation;
	int nClasses = 5; // 4 + 1
	int nSample = 2;
	int num = nRotation * nSample;
	int dim = nRotation * nClasses;
	int labels[] = {2, 2, 2,	1, 1, 1};
	int label;
	int p_len;
	float *p = create_p(p_len);
	int idx;
<<<<<<< HEAD
	float p_i, max_p_i;

	for(int n = 0; n < nSample; n++) {
		label = labels[n * nRotation];
		p_i = max_p_i = 0.0f;
		max_ang = 0;
		for(int i=0; i < nPose; i++) {
=======
	float tmp_prob, max_prob;
	int *labelR = new int[num];

	int *max_ang = new int[nSample];

	//print model

	cout << "nRotation(M) = " << nRotation << endl;
	cout << "nClasses = " << nClasses << endl;
	cout << "num =  nRotation * nSample = " << num << endl;
	cout << "dim =  nRotation * nClasses = " << dim << endl;
	cout << "============ Softmax Output ==============" << endl;
	for(int n = 0; n < nSample; n++) {
		for(int i=0; i < nRotation; i++) {
			cout << "  ";
			for(int j=0; j<nPose; j++) {
				for(int k=0; k<nClasses; k++) {
					cout << p[ (n * (dim * nRotation)) + (i * nRotation * nClasses) + (j * nClasses) + k ] << " ";
				}
				cout << "    ";
			}
			cout << endl;
		}
		cout << endl;
	}

	cout << "============================" << endl;
	for(int n = 0; n < nSample; n++) {
		label = labels[n * nRotation];
		max_prob = -FLT_MAX;
		max_ang[n] = -1;
		for(int i=0; i < nPose; i++) {
			tmp_prob = 0.0f;
>>>>>>> 79d708da567fab419a90a86dcb182989fd10c71c
			for(int j=0; j<nRotation; j++) {
				idx = i + j;
				if(idx >= nRotation)
					idx -= nRotation;
				//TODO: reduce this formula
<<<<<<< HEAD
				p_i += p[ (n * (dim * nRotation)) + (j * nRotation * nClasses) + (idx * nClasses) + label ];
			}
			if(p_i > max_p_i) {
				max_p_i = p_i;
				max_ang[n] = i;
			}
		}
	}
	
=======
				//cout << p[ (n * (dim * nRotation)) + (j * nRotation * nClasses) + (idx * nClasses) + label ] << endl;
				//cout << p[ (n * (dim * nRotation)) + (j * nRotation * nClasses) + (idx * nClasses) + (nClasses-1) ] << endl;
				tmp_prob += log(max(p[ (n * (dim * nRotation)) + (j * nRotation * nClasses) + (idx * nClasses) + label ], FLT_MIN))
				 		- log(max(p[ (n * (dim * nRotation)) + (j * nRotation * nClasses) + (idx * nClasses) + (nClasses-1) ], FLT_MIN));
			}
			// cout << "============================" << endl;
			if( tmp_prob > max_prob ) {
            	max_ang[ n ] = i;
            	max_prob = tmp_prob;
            }
		}
		cout << "max prob of sample " << n << ": " << max_prob << endl;
        cout << "max ang of sample " << n << ": " << max_ang[n] << endl;
        for(int i=0; i < nRotation; i++) {
			idx = i + max_ang[n];
			if(idx >= nRotation) idx -= nRotation;
			labelR[n * nRotation + idx] = idx * nClasses + label;
			cout << "image " << i << ": " << labelR[n * nRotation + idx] << ", ";
		}
		cout << endl;
		cout << "============================" << endl;
	}

	cout << "=========== Backward =======" << endl;

	delete[] max_ang;
	delete[] p;
	delete[] labelR;
>>>>>>> 79d708da567fab419a90a86dcb182989fd10c71c
	
	return 0;
}