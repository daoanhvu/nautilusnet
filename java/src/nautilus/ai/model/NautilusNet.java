package nautilus.ai.model;

import java.util.List;
import java.util.ArrayList;

public class NautilusNet {
	private double mLearningRate;
	private double[] biases;
	
	private double[] mInput;
	private double[] mTargets;
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
		//mTargets = new double[outputs.length];
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
		
		mTargets = new double[outputs.length];
		System.arraycopy(outputs, 0, mTargets, 0, outputs.length);
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
		double totalError = 0.0;
		//int outputSize = mTargets.length;
		
		for(i=1; i<mLayers.size(); i++) {
			layer = mLayers.get(i);
			layer.forward(mLayers.get(i-1));
		}
		
		//calculate the errors
		for(i=0; i<mTargets.length; i++) {
			mErrors[i] = mTargets[i] - mLayers.get(mTargets.length - 1).getNeuron(i).getOutput();
			totalError += (mErrors[i] * mErrors[i]) / 2.0;
		}
	}
	
	/**
	*	
	*/
	public void backward() {
		int i, j, k;
		int wn, l = mLayers.size();
		NNetLayer layer, hiddenLayer;
		NNeuron neuron;
		double tmpOut, out, w, dw, deltaout;
		
		//Calculate for the output layer
		layer = mLayers.get(l - 1);
		hiddenLayer = mLayers.get(l - 2);
		for(j=0; j<layer.size(); j++) {
			neuron = layer.getNeuron(j);
			tmpOut = neuron.getOutput();
			
			/*  d(E)/D(out) */
			deltaout = - (mTargets[j] - tmpOut);
			
			/*  d(out)/D(input) */
			out  = tmpOut * (1.0 - tmpOut);
			
			tmpOut = deltaout * out ;
			
			wn = neuron.getWeightCount();
			for(k=0; k<wn; k++) {
				dw = tmpOut * hiddenLayer.getNeuron(k).getOutput();
				w = neuron.getWeight(k);
				w = w - mLearningRate * dw;
				neuron.setWeight(w, k);
			}
		}
		
		//Calculate for hidden
		for(i=; i<hiddenLayer.size(); i++) {
			neuron = layer.getNeuron(i);
			
			//TODO:
			//dE/d(out(h[i]))
			for(j=0; j<mErrors.length; j++) {
			}
			
			wn = neuron.getWeightCount();
			for(k=0; k<wn; k++) {
				
			}
		}
	}
	
	@Override
	public String toString() {
		return "Size: " + mLayers.size() + "; Learning rate: " + mLearningRate;
	}
}