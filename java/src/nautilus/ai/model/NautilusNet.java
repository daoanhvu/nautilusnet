package nautilus.ai.model;

public class NautilusNet {
	private NNetLayer[] mLayers;
	private double mLearningRate;
	private double[] biases;
	
	private double[] mInput;
	private double[] mOutput;
	
	public NautilusNet() {
		mLayers = null;
		mLearningRate = 0.0;
		biases = null;
	}
	
	/**
	*	params
			rate this is learning rate of the network
			l number of layer in the net
	*/
	public NautilusNet(double rate, int l) {
		mLayers = new NNetLayer[l];
		biases = new double[l];
		mLearningRate = rate;
		
		//mInput = new double[inputs.length];
		//mOutput = new double[outputs.length];
	}
	
	public void setInputOutput(double[] inputs, double outputs) {
		mInput = new double[inputs.length];
		mOutput = new double[outputs.length];
		
		System.arrayCopy(inputs, 0, mInput, inputs.length);
		System.arrayCopy(outputs, 0, mOutput, outputs.length);
	}
	
	/**
	*	Get the ith layer from the net
	*/
	public NNetLayer getLayer(int i) {
		return mLayers[i];
	}
	
	public int size() {
		return mLayers.length;
	}
	
	public void setLearningRate(double rate) {
		mLearningRate = rate;
	}
	
	public void getLearningRate() {
		return mLearningRate;
	}
	
	public void forward() {
		int i;
		int j;
		
		NNetLayer layer;
		for(i=0; i<mLayers.length - 1; i++) {
			layer = mLayers[i];
			layer.forward(mLayers[i+1]);
		}
	}
	
	public void backward() {
		
	}
	
	@Override
	public String toString() {
		return "Size: " + mLayers.length;
	}
}