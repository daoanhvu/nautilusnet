#include <iostream>
#include <fstream>
#include <string>
#include <stdlib.h>

using namespace gm;
using namespace std;

/**
	Test multiply transpose matrix
*/
void softmax(const float *input, float *p, int len) {
	
}

void softmax_with_loss(const float *input, float *p, int len, float &loss) {

}

// Hard code to get fake data
float *create_p(int &len) {
	float v[] = {
		0.39f, 0.55f, 0.72f, 0.10f, 0.05f,	0.11f, 0.44f, 0.80f, 0.05, 0.09f,	0.35f, 0.21f, 0.63f, 0.19f, 0.17f,
		0.49f, 0.25f, 0.52f, 0.12f,	0.11f,	0.55f, 0.20f, 0.71f, 0.15, 0.15f,	0.45f, 0.24f, 0.53f, 0.21f, 0.19f,
		0.14f, 0.45f, 0.63f, 0.20f,	0.14f,	0.19f, 0.42f, 0.68f, 0.17, 0.14f,	0.29f, 0.29f, 0.58f, 0.15f, 0.02f,

		0.26f, 0.65f, 0.52f, 0.31f, 0.22f,	0.09f, 0.45f, 0.28f, 0.18, 0.17f,	0.40f, 0.55f, 0.50f, 0.42f, 0.35f,
		0.44f, 0.81f, 0.47f, 0.37f,	0.31f,	0.30f, 0.60f, 0.60f, 0.27, 0.20f,	0.19f, 0.61f, 0.22f, 0.39f, 0.18f,
		0.38f, 0.73f, 0.41f, 0.15f,	0.14f,	0.51f, 0.84f, 0.66f, 0.45, 0.32f,	0.31f, 0.43f, 0.13f, 0.40f, 0.21f
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
	float p_i, max_p_i;

	for(int n = 0; n < nSample; n++) {
		label = labels[n * nRotation];
		p_i = max_p_i = 0.0f;
		max_ang = 0;
		for(int i=0; i < nPose; i++) {
			for(int j=0; j<nRotation; j++) {
				idx = i + j;
				if(idx >= nRotation)
					idx -= nRotation;
				//TODO: reduce this formula
				p_i += p[ (n * (dim * nRotation)) + (j * nRotation * nClasses) + (idx * nClasses) + label ];
			}
			if(p_i > max_p_i) {
				max_p_i = p_i;
				max_ang[n] = i;
			}
		}
	}
	
	
	return 0;
}