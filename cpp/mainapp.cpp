#include "nautilusnet.h"
#include "fmat.hpp"

using namespace gm;

int main(int argc, char **args) {
	//NautilusNet *aNet = new NautilusNet(3, 400, 25, 10);
	//delete aNet;
	
	float **x = new float*[2];
	x[0] = new float[3];
	x[1] = new float[3];
	
	FMat<float> m(2, 3, x);
	
	cout << m;
	
	delete x[0];
	delete x[1];
	delete x;
	
	return 0;
}