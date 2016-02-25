package nautilus.ai.model;

import java.io.DataInputStream;
import java.io.DataOutputStream;
import java.io.File;
import java.io.FileInputStream;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.PrintStream;

import nautilus.ai.app.Application;

public class NautilusNet {
	private double mLearningRate;
	
	private double[] mInputLayer;
	private NNeuron[] mHiddenLayer;
	private NNeuron[] mOutputLayer;
	
	private double mBias1;
	private double mBias2;
	
	private double[] mTargets;
	private double[] mErrors;
	
	//This is used for backward
	private double[] dOutdNeto;
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
		mErrors = new double[outputCount];
		dOutdNeto = new double[outputCount];
		oldOuputWeights = new double[outputCount][hiddenCount];
	}
	
	public void setInputOutput(double[] inputs, double[] outputs) {
		
		if( (mInputLayer==null) || (inputs.length != mInputLayer.length)
			|| (mOutputLayer==null ) ) {
			throw new RuntimeException("Data does not match the network structure!!!!!!");
		}
		
		System.arraycopy(inputs, 0, mInputLayer, 0, inputs.length);
		
		for(int i=0; i<outputs.length; i++) {
			mErrors[i] = 0;
		}
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
	
	public double getTotalError() {
		int i;
		double err = 0;
		for(i=0; i<mOutputLayer.length; i++) {
			err += (mErrors[i]*mErrors[i])/2.0;
		}
		return err;
	}
	
	public void getErrors(double[] errs) {
		System.arraycopy(mErrors, 0, errs, 0, mOutputLayer.length);
	}
	
	public int getResultIndex() {
		int i;
		double err = 0;
		double minErr = (mErrors[0]*mErrors[0])/2.0;
		int minIdx = 0;
		for(i=1; i<mOutputLayer.length; i++) {
			err = (mErrors[i]*mErrors[i])/2.0;
			if(err < minErr) {
				minErr = err;
				minIdx = i;
			}
		}
		return minIdx;
	}
	
	public void forward() {
		int i;
		
		//calculate net and output values for the hidden layer
		//The last neuron of input layer is a bias
		for(i=0; i<mHiddenLayer.length; i++) {
			
			if(Application.getInstance().getDebugLevel()==Application.NETWORK_STEP) {
				System.out.println("***** Calculating for hidden node[" + i +"]:");
			}
			
			mHiddenLayer[i].onActivated(mInputLayer, mBias1);
		}
		
		//calculate net and output values for the output layer
		for(i=0; i<mOutputLayer.length; i++) {
			//Debug mode
			if(Application.getInstance().getDebugLevel()==Application.NETWORK_STEP) {
				System.out.println("***** Calculating for output node[" + i +"]:");
			}
			
			mOutputLayer[i].onActivated(mHiddenLayer, mBias2);
		}
		
		//calculate the errors
		for(i=0; i<mTargets.length; i++) {
			//mErrors[i] = mTargets[i] - mLayers.get(mTargets.length - 1).getNeuron(i).getOutput();
			//Now we calculate deltaErro = d(E)/d(output) = - ( target - output) = output - target
			mErrors[i] = mTargets[i] - mOutputLayer[i].getOutput();
			//totalError += (mErrors[i] * mErrors[i]) / 2.0;
			
			//For DEBUG mode
			if(Application.getInstance().getDebugLevel()==Application.NETWORK_STEP) {
				System.out.println("Error[" + i + "] = " + mErrors[i]);
			}
		}
	}
	
	/**
	*	
	*/
	public void backward() {
		int i, j, k;
		int wn;
		NNeuron neuron;
		double w, dw;
		double dOutHidden, dEttNet;
		
		//Calculate dw for the output layer
		for(j=0; j<mOutputLayer.length; j++) {
			neuron = mOutputLayer[j];

			/* thu cong thuc trong paper Neural Net Component */
			dOutdNeto[j] = (mTargets[j] - neuron.getOutput()) * neuron.getOutput() * (1.0 - neuron.getOutput()) ;
			
			wn = neuron.getWeightCount();
			for(k=0; k<wn; k++) {
				
				//First, we save the old weight;
				oldOuputWeights[j][k] = neuron.getWeight(k);
				
				/* output of node k(th) in hidden layer is the input k(th) in output layer  */
				dw = dOutdNeto[j] * mHiddenLayer[k].getOutput();
				w = neuron.getWeight(k) - mLearningRate * dw;
				neuron.setWeight(w, k); // <- Should we update weight of output right here?
				
				if(Application.getInstance().getDebugLevel()==Application.NETWORK_STEP) {
					System.out.println("dw = " + mLearningRate + " * " + mErrors[j] + 
							" * " + mHiddenLayer[k].getOutput() + " * " + neuron.getOutput() + 
							" * (1 - " + neuron.getOutput() + ") = " + (dw*mLearningRate));
				}
			}
		}
		
		//Calculate dw for hidden layer
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
				dEttNet += dOutdNeto[j] * oldOuputWeights[j][i];
			}
			
			wn = neuron.getWeightCount();
			for(k=0; k<wn; k++) {
				dw = dEttNet * dOutHidden * mInputLayer[k];
				w = neuron.getWeight(k) - mLearningRate * dw;
				neuron.setWeight(w, k); // <- Should we update weight of output right here?
			}
		}
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
	
	/**
		This is for testing purpose
	*/
	public void printNetwork(PrintStream out)  {
		int i;
		out.println("*************************************************************************");
		out.println("Learning rate: " + mLearningRate);
		out.println("Input: ");
		for(i = 0; i<mInputLayer.length; i++) {
			out.print(mInputLayer[i] + " | ");
		}
		out.println();
		
		out.println("***INPUT LAYER: ");
		for(i = 0; i<mInputLayer.length; i++) {
			out.print(mInputLayer[i] + "");
			out.println();
		}
		out.println();
		
		out.println("***HIDDEN LAYER: ");
		for(i = 0; i<mHiddenLayer.length; i++) {
			mHiddenLayer[i].print(out);
			out.println();
		}
		out.println();
		
		out.println("***OUTPUT LAYER: ");
		for(i = 0; i<mOutputLayer.length; i++) {
			mOutputLayer[i].print(out);
			out.println();
		}
		out.println();
		
		out.println("Target: ");
		for(i = 0; i<mTargets.length; i++) {
			out.print(mTargets[i] + " | ");
		}
		out.println();
	}
	
	@Override
	public String toString() {
		return "Size: ; Learning rate: " + mLearningRate;
	}
}