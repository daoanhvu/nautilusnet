package nautilus.ai.model;

public class NNeuron {
	/**
	*	Remember that a weight is the connecting value from this neuron
	*	to a neuron in the next layer, thus, neurons of the output layer have no weight
	*/
	private double[] weights;
	
	public double getWeight(int index) {
		return weights[index];
	}
	
	public void setWeight(double w, int index) {
		weight[index] = w;
	}
}