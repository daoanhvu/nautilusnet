package nautilus.ai.model;

import java.io.PrintStream;
import java.util.List;
import java.util.ArrayList;

public class NautilusNet {
	private double mLearningRate;
	private double[] biases;
	
	private double[] mInput;
	private double[] mTargets;
	private double[] mErrors;
	private double[] mDeltaE;
	
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
		//double totalError = 0.0;
		//int outputSize = mTargets.length;
		
		for(i=1; i<mLayers.size(); i++) {
			layer = mLayers.get(i);
			layer.forward(mLayers.get(i-1));
		}
		
		//calculate the errors
		for(i=0; i<mTargets.length; i++) {
			//mErrors[i] = mTargets[i] - mLayers.get(mTargets.length - 1).getNeuron(i).getOutput();
			//Now we calculate deltaErro = d(E)/d(output) = - ( target - output) = output - target
			mErrors[i] = mLayers.get(mTargets.length - 1).getNeuron(i).getOutput() - mTargets[i];
			//totalError += (mErrors[i] * mErrors[i]) / 2.0;
		}
	}
	
	/**
	*	
	*/
	public void backward() {
		int i, j, k;
		int wn, l = mLayers.size();
		NNetLayer outLayer, hiddenLayer, inputLayer;
		NNeuron neuron;
		double tmpOut, out, w, dw;
		double dOutHidden, dEttNet, inputValue;
		double dOut[] = new double[mErrors.length];
		double dEdNetHidden[] = new double[mErrors.length];
		double dOutDNet[] = new double[mErrors.length];
		
		//Calculate for the output layer
		outLayer = mLayers.get(l - 1);
		hiddenLayer = mLayers.get(l - 2);
		for(j=0; j<outLayer.size(); j++) {
			neuron = outLayer.getNeuron(j);
			tmpOut = neuron.getOutput();
			
			/*  d(out)/D(input) */
			dOut[j]  = tmpOut * (1.0 - tmpOut);
			
			tmpOut = mErrors[j] * dOut[j] ;
			
			wn = neuron.getWeightCount();
			for(k=0; k<wn; k++) {
				dw = tmpOut * hiddenLayer.getNeuron(k).getOutput();
				w = neuron.getWeight(k);
				w = w - mLearningRate * dw;
				neuron.setWeight(w, k); // <- Should we update weight of output right here?
			}
		}
		
		//Calculate for hidden
		for(i=0; i<hiddenLayer.size(); i++) {
			neuron = hiddenLayer.getNeuron(i);
			
			//TODO:
			
			//d(outHidden)/d(netHidden) = outHidden (1 - outHidden)
			dOutHidden = neuron.getOutput() * (1 - neuron.getOutput());
			
			/**
				d(E)/d(out(h[i])) = d(E) / d(netOut) * d(netOut) / d(outHidden)
				
				d(E) / d(net) = d(E)/d(out) * d(out)/d(net)
			*/
			dEttNet = 0;
			for(j=0; j<mErrors.length; j++) {
				dEdNetHidden[j] = mErrors[j] * dOut[j];
				dOutDNet[j] =  outLayer.getNeuron(j).getWeight(i);
				dEttNet += dEdNetHidden[j] * dOutDNet[j];
				
				
			}
			
			inputLayer = mLayers.get(0);
			wn = neuron.getWeightCount();
			for(k=0; k<wn; k++) {
				inputValue = inputLayer.getNeuron(k).getInput();
				dw = dEttNet * dOutHidden * inputValue;
				w = neuron.getWeight(k);
				w = w - mLearningRate * dw;
				neuron.setWeight(w, k); // <- Should we update weight of output right here?
			}
		}
	}
	
	/**
		This is for testing purpose
	*/
	public void printNetwork(PrintStream out)  {
		int i;
		out.println("*************************************************************************");
		out.println("Learning rate: " + mLearningRate);
		out.println("Input: ");
		for(i = 0; i<mInput.length; i++) {
			out.print(mInput[i] + " | ");
		}
		
		for(i = 0; i<mLayers.size(); i++) {
			mLayers.get(i).print(out);
		}
		
		out.println("Target: ");
		for(i = 0; i<mTargets.length; i++) {
			out.print(mTargets[i] + " | ");
		}
	}
	
	@Override
	public String toString() {
		return "Size: " + mLayers.size() + "; Learning rate: " + mLearningRate;
	}
}