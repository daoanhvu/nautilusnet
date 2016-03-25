package nautilus.ai.model;

import java.io.PrintStream;

/**
* Dao Anh Vu
*
*/
public class NNeuron {
	/**
		At input layer, this value is called input value.
		At hidden layer, this value is call net value.
	*/
	private double mInput;
	private double mOutput;
	
	/**
	*	Remember that a weight is the connecting value from a neuron in the previous layer to this neuron,
	*	thus, neurons of the input layer have no weight
	*/
	private double[] mWeights;
	
	public NNeuron() {
	}
	
	public NNeuron(int weightLength) {
		mWeights = new double[weightLength];
	}
	
	public NNeuron(double[] initWeights) {
		mWeights = new double[initWeights.length];
		System.arraycopy(initWeights, 0, mWeights, 0, initWeights.length);
	}
	
	public int getWeightCount() {
		return mWeights.length;
	}
	
	public double getWeight(int index) {
		return mWeights[index];
	}
	
	public void setWeight(double w, int index) {
		mWeights[index] = w;
	}
	
	public void setWeightArray(double[] initWeights) {
		if(initWeights.length < mWeights.length)
			throw new RuntimeException("List weight not enough. Need " + mWeights.length + "; got: " + initWeights.length);
		
		System.arraycopy(initWeights, 0, mWeights, 0, mWeights.length);
	}
	
	public void setInput(double input) {
		mInput = input;
	}
	
	public void setOutput(double v) {
		mOutput = v;
	}
	
	public double getInput() {
		return mInput;
	}
	
	public double getOutput() {
		return mOutput;
	}
	
	/**
	*	return output
	*/
	public double onActivated(final double[] preLayerOutput, double bias) {
		int i;
		mInput = 0;
		for(i=0; i<mWeights.length; i++) {
			mInput += preLayerOutput[i] * mWeights[i];
		}
		mOutput = 1.0 / (1.0 + Math.exp(-mInput + bias));
		return mOutput;
	}
	
	public double onActivated(final NNeuron[] preLayer, double bias) {
		int i;
		mInput = 0;
		for(i=0; i<mWeights.length; i++) {
			mInput += preLayer[i].getOutput() * mWeights[i];
		}
		mOutput = 1.0 / (1.0 + Math.exp(-mInput + bias));
		return mOutput;
	}
}