package nautilus.ai.model;

/**
* Dao Anh Vu
*
*/
public class NNeuron {
	/**
	*	Remember that a weight is the connecting value from a neuron in the next layer to this neuron,
	*	thus, neurons of the input layer have no weight
	*/
	private double mInput;
	private double mOutput;
	private double[] mWeights;
	private double mBias;
	
	public double getWeight(int index) {
		return mWeights[index];
	}
	
	public void setWeight(double w, int index) {
		mWeights[index] = w;
	}
	
	public void setInput(double input) {
		mInput = input;
	}
	
	public double getOutput() {
		return mOutput;
	}
	
	/**
	*	return output
	*/
	public double onActivated(final NNetLayer preLayer) {
		int i;
		double[] inputValues = new double[mWeights.length];
		double s = 0;
		for(i=0; i<mWeights.length; i++) {
			s += preLayer.getNeuron(i).getOutput() * mWeights[i];
		}
		
		mOutput = 1.0 / (1.0 + Math.exp(-s));
		
		return mOutput;
	}
}