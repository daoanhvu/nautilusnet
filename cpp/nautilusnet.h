#ifndef _NAUTILUSNET_H
#define_NAUTILUSNET_H

class NautilusNet {
	private:
		/* Input layer */
		double* mInputs;
		
		/* hidden layer */
		double* mNet1s;
		double* mWeights1;
		double* mOutput1s;
		
		/* output layer */
		double* mNet2s;
		double* mWeights2;
		double* mOutput2s;
		
		double* mTargets;
		double* mDErrors;
		
	public:
		NautilusNet();
		
		void setInputOutput(const double *inputs, const double* ouputs);
		void forward();
		void backward();
		
};

#endif