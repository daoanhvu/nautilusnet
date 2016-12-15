#include <iostream>
#include "nautilusnet.h"
#include "fmat.hpp"

using namespace gm;
using namespace std;

int main(int argc, char **args) {
	NautilusNet *aNet = new NautilusNet(3, 400, 25, 10);
	delete aNet;
	return 0;
}