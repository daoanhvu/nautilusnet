package nautilus.ai.model;

import java.util.List;
import java.util.ArrayList;

public class NautilusNet {
	private double mLearningRate;
	private double[] biases;
	
	private double[] mInput;
	private double[] mOutput;
	private double[] mErrors;
	
	private List<NNetLayer> mLayers = new ArrayList<NNetLayer>();
	
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
		biases = new double[l];
		mLearningRate = rate;
		
		//mInput = new double[inputs.length];
		//mOutput = new double[outputs.length];
	}
	
	public void setInputOutput(double[] inputs, double[] outputs) {
		
		if( (mLayers.size()==0) || (inputs.length != mLayers.get(0).size()) ) {
			throw new RuntimeException("");
		}
		NNeuron n;
		for(int i=0; i<mLayers.get(0).size(); i++) {
			n = mLayers.get(0).getNeuron(i);
			n.setInput(inputs[i]);
		}
		
		mOutput = new double[outputs.length];
		System.arraycopy(outputs, 0, mOutput, 0, outputs.length);
	}
		
	//Setup hidden layer
	public void addLayer(NNetLayer layer) {
		mLayers.add(layer);
	}
	
	/**
	*	Get the ith layer from the net
	*/
	public NNetLayer getLayer(int i) {
		return mLayers.get(i);
	}
	
	public int size() {
		return mLayers.size();
	}
	
	public void setLearningRate(double rate) {
		mLearningRate = rate;
	}
	
	public double getLearningRate() {
		return mLearningRate;
	}
	
	public void forward() {
		int i;
		int j;
		NNetLayer layer;
		//int outputSize = mOutput.length;
		
		for(i=1; i<mLayers.size(); i++) {
			layer = mLayers.get(i);
			layer.forward(mLayers.get(i-1));
		}
		
		//calculate the errors
		for(i=0; i<mOutput.length; i++) {
			mErrors[i] = mOutput[i] - mLayers.get(mOutput.length - 1).getOutput();
		}
	}
	
	public void backward() {
		
	}
	
	@Override
	public String toString() {
		return "Size: " + mLayers.size() + "; Learning rate: " + mLearningRate;
	}
}