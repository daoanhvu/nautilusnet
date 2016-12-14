#ifndef _NAUTILUSNET_H
#define_NAUTILUSNET_H

class NautilusNet {
	private:
		/* Input layer */
		double* mInputs;
        int inputSize;
		
		/* hidden layer */
		double* mNet1s;
        /**
            weight1 is a matrix with number of row equal to number of node in hidden layer and
            number of column is the number of input node.
        */
		double* mWeights1;
		double* mOutput1s;
        int mHiddenSize;
		
		/* output layer */
		double* mNet2s;
		double* mWeights2;
		double* mOutput2s;
        int numLabel;
		
		double* mTargets;
		double* mDErrors;
		
	public:
		NautilusNet();
        ~NautilusNet();
		
		void setInputOutput(const double *inputs, const double* ouputs);
		void forward();
		void backward();
        double sigmoid();
        void costFunction(double lambda, double J, double *grad, int &ng);
		
};

#endif