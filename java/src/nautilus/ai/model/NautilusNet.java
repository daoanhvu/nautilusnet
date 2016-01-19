package nautilus.ai.model;

import java.io.PrintStream;

public class NautilusNet {
	private double mLearningRate;
	
	private NNeuron[] mInputLayer;
	private NNeuron[] mHiddenLayer;
	private NNeuron[] mOutputLayer;
	
	private double[] mTargets;
	private double[] mErrors;
	private double[] mDeltaE;
	
	public NautilusNet() {
		mLearningRate = 0.0;
	}
	
	/**
	*	params
			rate this is learning rate of the network
			l number of layer in the net
	*/
	public NautilusNet(double rate, int inputCount, int hiddenCount, int outputCount) {
		mLearningRate = rate;
		
		mInputLayer = new NNeuron[inputCount];
		mHiddenLayer = new NNeuron[hiddenCount];
		mOutputLayer = new NNeuron[outputCount];
		
		mTargets = new double[outputCount];
	}
	
	public void setInputOutput(double[] inputs, double[] outputs) {
		
		if( (mInputLayer==null) || (inputs.length != mInputLayer.length)
			|| (mOutputLayer==null ) ) {
			throw new RuntimeException("Data does not match the network structure!!!!!!");
		}
		NNeuron n;
		for(int i=0; i<mInputLayer.length; i++) {
			mInputLayer[i].setInput(inputs[i]);
		}
		
		mTargets = new double[outputs.length];
		System.arraycopy(outputs, 0, mTargets, 0, outputs.length);
	}
		
	//Setup hidden layer
	public void setInputLayer(NNeuron[] inputLayer) {
		mInputLayer = inputLayer;
	}
	
	public NNeuron[] getInputLayer() {
		return mInputLayer;
	}
	
	public void setHiddenLayer(NNeuron[] hLayer) {
		mHiddenLayer = hLayer;
	}
	
	public NNeuron[] getHiddenLayer() {
		return mHiddenLayer;
	}
	
	public void setOutputLayer(NNeuron[] oLayer) {
		mOutputLayer = oLayer;
	}
	
	public NNeuron[] getOutputLayer() {
		return mOutputLayer;
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
		
		//calculate net and ouput values for the hidden layer
		//The last neuron of input layer is a bias
		for(i=0; i<mHiddenLayer.length-1; i++) {
			mHiddenLayer[i].onActivated(mInputLayer);
		}
		
		//calculate net and ouput values for the output layer
		for(i=0; i<mOutputLayer.length; i++) {
			mOutputLayer[i].onActivated(mHiddenLayer);
		}
		
		//calculate the errors
		for(i=0; i<mTargets.length; i++) {
			//mErrors[i] = mTargets[i] - mLayers.get(mTargets.length - 1).getNeuron(i).getOutput();
			//Now we calculate deltaErro = d(E)/d(output) = - ( target - output) = output - target
			mErrors[i] = mOutputLayer[i].getOutput() - mTargets[i];
			//totalError += (mErrors[i] * mErrors[i]) / 2.0;
		}
	}
	
	/**
	*	
	*/
	public void backward() {
		int i, j, k;
		int wn;
		NNeuron neuron;
		double tmpOut, out, w, dw;
		double dOutHidden, dEttNet, inputValue;
		double dOut[] = new double[mErrors.length];
		double dEdNetHidden[] = new double[mErrors.length];
		double dOutDNet[] = new double[mErrors.length];
		
		//Calculate for the output layer
		for(j=0; j<mOutputLayer.length; j++) {
			neuron = mOutputLayer[j];
			tmpOut = neuron.getOutput();
			
			/*  d(out)/D(input) */
			dOut[j]  = tmpOut * (1.0 - tmpOut);
			
			tmpOut = mErrors[j] * dOut[j] ;
			
			wn = neuron.getWeightCount();
			for(k=0; k<wn; k++) {
				dw = tmpOut * mHiddenLayer[k].getOutput();
				w = neuron.getWeight(k);
				w = w - mLearningRate * dw;
				neuron.setWeight(w, k); // <- Should we update weight of output right here?
			}
		}
		
		//Calculate for hidden
		for(i=0; i<mHiddenLayer.length-1; i++) {
			neuron = mHiddenLayer[i];
			
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
				dOutDNet[j] =  mOutputLayer[j].getWeight(i);
				dEttNet += dEdNetHidden[j] * dOutDNet[j];
			}
			
			wn = neuron.getWeightCount();
			for(k=0; k<wn; k++) {
				inputValue = mInputLayer[k].getInput();
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
		for(i = 0; i<mInputLayer.length; i++) {
			out.print(mInputLayer[i].getInput() + " | ");
		}
		
		for(i = 0; i<mInputLayer.length; i++) {
			mInputLayer[i].print(out);
		}
		
		for(i = 0; i<mHiddenLayer.length; i++) {
			mHiddenLayer[i].print(out);
		}
		
		for(i = 0; i<mOutputLayer.length; i++) {
			mOutputLayer[i].print(out);
		}
		
		out.println("Target: ");
		for(i = 0; i<mTargets.length; i++) {
			out.print(mTargets[i] + " | ");
		}
	}
	
	@Override
	public String toString() {
		return "Size: ; Learning rate: " + mLearningRate;
	}
}