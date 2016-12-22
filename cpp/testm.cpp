#include <iostream>
#include "fmat.hpp"

using namespace gm;
using namespace std;

int main(int argc, char **args) {
	double vs[3] = {3.6, 1.2, 2.0};
	double ms[6] = {5.2, 2.0, 1.4, 0.7, 4.1, 0.5};
	FMat<double> m(ms, 2, 3);
	Vec<double> v(vs, 3);

	cout << m << endl;
	cout << v << endl;
	
	//Vec<double> t = m * v;
	Vec<double> t;
	t = m * v;
	
	cout << t << endl;
	
	return 0;
}