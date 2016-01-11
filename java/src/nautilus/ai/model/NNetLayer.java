package nautilus.ai.model;

public class NNetLayer {
	private NNeuron[] mNeurons;
	
	public NNetLayer(int numOfNeuron) {
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
}