#ifndef _NAUTILUSNET_H
#define_NAUTILUSNET_H

class NautilusNet {
	private:
		double* mWeights1;
		double* mWeights2;
		
		double* mInputs;
		double* mTargets;
		double* mOutputs;
		double* mDErrors;
		
	public:
		NautilusNet();
		
		void setInputOutput(const double *inputs, const double* ouputs);
		void forward();
		void backward();
		
};

#endif