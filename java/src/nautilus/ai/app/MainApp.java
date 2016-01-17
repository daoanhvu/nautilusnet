package nautilus.ai.app;

import nautilus.ai.model.NautilusNet;

public class MainApp {
	public static void main(String[] agrs) {
		NautilusNet aNet = new NautilusNet(0.5, 3, 3, 2);
		
		//Setup input layer
		//2 input layer + 1 bias
		//NNetLayer inputLayer = new NNetLayer(3);
		//aNet.addLayer(inputLayer);
		
		//Setup hidden layer
		//NNetLayer hiddenLayer = new NNetLayer(2);
		//hiddenLayer.getNeuron(0).setWeightArray(new double[] {0.15, 0.25, 0.35} );
		//hiddenLayer.getNeuron(1).setWeightArray(new double[] {} );
		//hiddenLayer.getNeuron(0).setWeightArray(new double[] {} ); // <- this is bias
		//aNet.addLayer(hiddenLayer);
		
		//Setup output layer
		//2 node + 1 bias
		//NNetLayer outputLayer = new NNetLayer(1);
		//outputLayer.getNeuron(0).setWeightArray(new double[] {} );
		//outputLayer.getNeuron(1).setWeightArray(new double[] {} );
		//outputLayer.getNeuron(0).setWeightArray(new double[] {} ); // <- this is bias
		//aNet.addLayer(outputLayer);
		
		//train the net
		double[] inputs = new double[] {0.05, 0.1, 1};
		double[] targets = new double[] {0.01, 0.99};
	}
}