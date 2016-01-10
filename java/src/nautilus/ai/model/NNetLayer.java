package nautilus.ai.model;

public abstract class NNetLayer {
	private NNeuron[] mNeurons;
	
	public void forward(NNetLayer nextLayer) {
		int i;
		int ns = nextLayer.length;
		
		for(i=0; i<mNeurons.length; i++) {
			for(j=0; j<ns; j++) {
				
			}
		}
	}
}