package nautilus.ai.app;

import nautilus.ai.model.NautilusNet;
import nautilus.ai.ui.ImageForm;

import javax.swing.SwingUtilities;

import nautilus.ai.model.NNeuron;

public class MainApp {
	
	public static void main(String[] agrs) {
		
		//Load configurations
		Application.getInstance();
		
		SwingUtilities.invokeLater(new Runnable() {
            @Override
            public void run() {
            	ImageForm mainForm = new ImageForm();
            	mainForm.setVisible(true);
            }
        });
		
//		testANN();
//		testAND();
	}
	
	static void testANN() {
		/*
		Now we test a ANN with properties below:
			- learning rate: 0.5
			- Input: 2 + bias
			- Hidden: 2 + bias
			- output: 2
		*/
		NautilusNet aNet = new NautilusNet(0.5, 2, 2, 2);
		int i, j;
		NNeuron neuron;
		
		/**
		 * The number of row is the number of node in hidden layer
		 * Each row contain the weights those connect from input layer to corresponding
		 * node in hidden layer.
		 * Ex: in the array iniW1 below, data in row 0 {0.15, 0.20, 0.35} is the weights
		 * those connect from all node in input layer to node 0 of hidden layer. 
		 */
		double[][] initW1 = {
							{0.15, 0.20},
							{0.25, 0.30}
							};
							
		double[][] initW2 = {
							{0.40, 0.45},
							{0.50, 0.55}
							};
		
		aNet.setBiases(.35, .60);
		//Setup hidden layer		
		for(i=0; i<2; i++) {
			neuron = aNet.getHiddenLayer()[i];
			for(j=0; j<2; j++) {
				neuron.setWeight(initW1[i][j], j);
			}
		}
		
		//Setup output layer
		for(i=0; i<2; i++) {
			neuron = aNet.getOutputLayer()[i];
			for(j=0; j<2; j++) {
				neuron.setWeight(initW2[i][j], j);
			}
		}
		
		//train the net
		double[] inputs = new double[] {0.05, 0.1};
		double[] targets = new double[] {0.01, 0.99};
		
		aNet.setInputOutput(inputs, targets);
		
		aNet.forward();
		aNet.backward();
	}
	
	static void testAND() {
		/*
		Now we test a ANN with properties below:
			- learning rate: 0.25
			- Input: 2
			- Hidden: 2
			- output: 1
		*/
		NautilusNet aNet = new NautilusNet(.25, 2, 2, 1);
		NNeuron neuron;
		
		aNet.setBiases(0.0, 0.0);
		
		//Setup hidden layer		
		neuron = aNet.getHiddenLayer()[0];
		neuron.setWeight(0.62, 0);
		neuron.setWeight(0.55, 1);
		
		neuron = aNet.getHiddenLayer()[1];
		neuron.setWeight(0.42, 0);
		neuron.setWeight(-0.17, 1);
		
		//Setup output layer
		neuron = aNet.getOutputLayer()[0];
		neuron.setWeight(0.35, 0);
		neuron.setWeight(0.81, 1);
		
		
		//train the net 0 and 1 = 0
		double[] inputs = new double[] {0, 1};
		double[] targets = new double[] {0.0};
		
		aNet.setInputOutput(inputs, targets);
		aNet.forward();
		
		System.out.println("CALCULATE FOR BACKWARD \n");
		
		aNet.backward();
		
		
//		
//		inputs = new double[] {1, 1};
//		targets = new double[] {1};
//		
//		aNet.setInputOutput(inputs, targets);
//		aNet.forward();
//		aNet.printNetwork(System.out);
//		aNet.backward();
//		aNet.printNetwork(System.out);
	}
}