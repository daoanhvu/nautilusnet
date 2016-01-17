package nautilus.ai.model;

import java.io.PrintStream;

public class NNetLayer {
	private NNeuron[] mNeurons;
	
	public NNetLayer(int numOfNeuron, double[] w) {
		int i;
		mNeurons = new NNeuron[numOfNeuron];
		for(i=0; i<numOfNeuron; i++) {
			mNeurons[i] = new NNeuron();
		}
	}
	
	public void forward(NNetLayer preLayer) {
		int i;
		for(i=0; i<mNeurons.length; i++) {
			mNeurons[i].onActivated(preLayer);
		}
	}
	
	public NNeuron getNeuron(int index) {
		return mNeurons[index];
	}
	
	public int size() {
		return mNeurons.length;
	}
	
	public void print(PrintStream out) {
		int i;
		for(i=0; i<mNeurons.length; i++) {
			mNeurons[i].print(out);
			out.print(" ");
		}
	}
}