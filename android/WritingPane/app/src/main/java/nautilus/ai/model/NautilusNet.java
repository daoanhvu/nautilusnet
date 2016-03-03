package nautilus.ai.model;

import java.io.DataInputStream;
import java.io.DataOutputStream;
import java.io.File;
import java.io.FileInputStream;
import java.io.FileOutputStream;
import java.io.IOException;

public class NautilusNet {
	private double mLearningRate;
	
	private double[] mInputLayer;
	private NNeuron[] mHiddenLayer;
	private NNeuron[] mOutputLayer;
	
	private double mBias1;
	private double mBias2;
	
	private double[] mTargets;
	
	//This is used for backward
	private double[] delta;
	private double[][] oldOuputWeights;
	
	public NautilusNet() {
		mLearningRate = 0.0;
		mBias1 = mBias2 = 0;
	}
	
	/**
	*	params
			rate this is learning rate of the network
			l number of layer in the net
	*/
	public NautilusNet(double rate, int inputCount, int hiddenCount, int outputCount) {
		mLearningRate = rate;
		
		mInputLayer = new double[inputCount];
		mHiddenLayer = new NNeuron[hiddenCount];
		mOutputLayer = new NNeuron[outputCount];
		
		for(int i=0; i<mHiddenLayer.length; i++) {
			mHiddenLayer[i] = new NNeuron(inputCount);
		}
		
		for(int i=0; i<mOutputLayer.length; i++) {
			mOutputLayer[i] = new NNeuron(hiddenCount);
		}
		
		mTargets = new double[outputCount];
		delta = new double[outputCount];
		oldOuputWeights = new double[outputCount][hiddenCount];
	}
	
	public void setInputOutput(double[] inputs, double[] outputs) {
		
		if( (mInputLayer==null) || (inputs.length != mInputLayer.length)
			|| (mOutputLayer==null ) ) {
			throw new RuntimeException("Data does not match the network structure!!!!!!");
		}
		
		System.arraycopy(inputs, 0, mInputLayer, 0, inputs.length);
		System.arraycopy(outputs, 0, mTargets, 0, outputs.length);
	}
	
	public void setBiases(double bias1, double bias2) {
		mBias1 = bias1;
		mBias2 = bias2;
	}
	
	public double getBias1() {
		return mBias1;
	}
	
	public double getBias2() {
		return mBias2;
	}
	
	public void setInput(double[] inputs) {
		
		if( (mInputLayer==null) || (inputs.length != mInputLayer.length)
			|| (mOutputLayer==null ) ) {
			throw new RuntimeException("Data does not match the network structure!!!!!!");
		}
		
		System.arraycopy(inputs, 0, mInputLayer, 0, inputs.length);
	}
		
	//Setup hidden layer
	public void setInputLayer(double[] inputLayer) {
		mInputLayer = inputLayer;
	}
	
	public double[] getInputLayer() {
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
	
	public int getResultIndex() {
		int i;
		double max = mOutputLayer[0].getOutput();
		int maxIdx = 0;
		for(i=1; i<mOutputLayer.length; i++) {
			if(max < mOutputLayer[i].getOutput()) {
				max = mOutputLayer[i].getOutput();
				maxIdx = i;
			}
		}
		return maxIdx;
	}
	
	public void forward() {
		int i;
		
		//calculate net and output values for the hidden layer
		//The last neuron of input layer is a bias
		for(i=0; i<mHiddenLayer.length; i++) {
			mHiddenLayer[i].onActivated(mInputLayer, mBias1);
		}
		
		//calculate net and output values for the output layer
		for(i=0; i<mOutputLayer.length; i++) {
			mOutputLayer[i].onActivated(mHiddenLayer, mBias2);
		}
	}
	
	/**
	*	
	*/
	public double backward() {
		int i, j, k;
		int wn;
		NNeuron neuron;
		double w, error, totalError=0;
		double dOutHidden, dEttNet;
		
		//Calculate delta for the output layer
		for(j=0; j<mOutputLayer.length; j++) {
			neuron = mOutputLayer[j];

			/* thu cong thuc trong paper Neural Net Component */
			error = neuron.getOutput() - mTargets[j];
			totalError += error * error;
			delta[j] = error * neuron.getOutput() * (1.0 - neuron.getOutput()) ;
			
			wn = neuron.getWeightCount();
			for(k=0; k<wn; k++) {
				//First, we save the old weight;
				oldOuputWeights[j][k] = neuron.getWeight(k);
				/* output of node k(th) in hidden layer is the input k(th) in output layer  */
				w = neuron.getWeight(k) - mLearningRate * delta[j] * mHiddenLayer[k].getOutput();
				neuron.setWeight(w, k); // <- Should we update weight of output right here?
			}
		}
		
		//Calculate delta for hidden layer
		for(i=0; i<mHiddenLayer.length; i++) {
			neuron = mHiddenLayer[i];
			
			//d(outHidden)/d(netHidden) = outHidden (1 - outHidden)
			dOutHidden = neuron.getOutput() * (1 - neuron.getOutput());
			
			/**
				d(E)/d(out(h[i])) = d(E) / d(netOut) * d(netOut) / d(outHidden)				
				d(E) / d(net) = d(E)/d(out) * d(out)/d(net)
			*/
			dEttNet = 0;
			for(j=0; j<mOutputLayer.length; j++) {
				//should we use old weight or new weight here?   
				dEttNet += delta[j] * oldOuputWeights[j][i];
			}
			
			wn = neuron.getWeightCount();
			for(k=0; k<wn; k++) {
				w = neuron.getWeight(k) - mLearningRate * dEttNet * dOutHidden * mInputLayer[k];
				neuron.setWeight(w, k); // <- Should we update weight of output right here?
			}
		}

		return (totalError/2.0);
	}
	
	public void writeWeight2File(String filepath) {
		FileOutputStream fos = null;
		DataOutputStream dos = null;
		int i, j;
		try {
			fos = new FileOutputStream(new File(filepath));
			dos = new DataOutputStream(fos);
			
			for(i=0; i<mHiddenLayer.length; i++) {
				for(j=0; j<mHiddenLayer[i].getWeightCount(); j++) {
					dos.writeDouble(mHiddenLayer[i].getWeight(j));
				}
			}
			
			for(i=0; i<mOutputLayer.length; i++) {
				for(j=0; j<mOutputLayer[i].getWeightCount(); j++) {
					dos.writeDouble(mOutputLayer[i].getWeight(j));
				}
			}
			dos.flush();
			
		} catch(IOException ex) {
			ex.printStackTrace();
		} finally {
			try {
				if(fos != null) fos.close();
			} catch(IOException ex) {	}
		}
	}
	
	public void readWeightFromFile(String filepath) {
		FileInputStream fis = null;
		DataInputStream dis = null;
		int i, j;
		try {
			fis = new FileInputStream(new File(filepath));
			dis = new DataInputStream(fis);
			
			for(i=0; i<mHiddenLayer.length; i++) {
				for(j=0; j<mHiddenLayer[i].getWeightCount(); j++) {
					mHiddenLayer[i].setWeight(dis.readDouble(), j);
				}
			}
			
			for(i=0; i<mOutputLayer.length; i++) {
				for(j=0; j<mOutputLayer[i].getWeightCount(); j++) {
					mOutputLayer[i].setWeight(dis.readDouble(), j);
				}
			}
		} catch(IOException ex) {
			ex.printStackTrace();
		} finally {
			try {
				if(fis != null) fis.close();
			} catch(IOException ex) {	}
		}
	}

	public void readWeightInputStream(DataInputStream dis) {
		FileInputStream fis = null;
		int i, j;
		double temp;
		try {
			dis = new DataInputStream(fis);

			//read learning rate
			mLearningRate = dis.readDouble();

			//read biases
			mBias1 = dis.readDouble();
			mBias2 = dis.readDouble();

			for(i=0; i<mHiddenLayer.length; i++) {
				for(j=0; j<mHiddenLayer[i].getWeightCount(); j++) {
					mHiddenLayer[i].setWeight(dis.readDouble(), j);
				}
			}

			for(i=0; i<mOutputLayer.length; i++) {
				for(j=0; j<mOutputLayer[i].getWeightCount(); j++) {
					mOutputLayer[i].setWeight(dis.readDouble(), j);
				}
			}
		} catch(IOException ex) {
			ex.printStackTrace();
		} finally {
			try {
				if(fis != null) fis.close();
			} catch(IOException ex) {	}
		}
	}
	
	@Override
	public String toString() {
		return "Size: ; Learning rate: " + mLearningRate;
	}
}