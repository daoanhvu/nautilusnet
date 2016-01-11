package nautilus.ai.app;

import nautilus.ai.model.NautilusNet;
import nautilus.ai.model.NNetLayer;

public class MainApp {
	public static void main(String[] agrs) {
		NautilusNet aNet = new NautilusNet(0.25, 3);
		
		//Setup input layer
		NNetLayer inputLayer = new NNetLayer(2);
		aNet.addLayer(inputLayer);
		
		//Setup hidden layer
		NNetLayer hiddenLayer = new NNetLayer(2);
		aNet.addLayer(hiddenLayer);
		
		//Setup output layer
		NNetLayer outputLayer = new NNetLayer(1);
		aNet.addLayer(outputLayer);
		
		//train the net
		double[] inputs = new double[] {0, 1, 1, 1};
		double[] output = new double[] {0, 1};
	}
}