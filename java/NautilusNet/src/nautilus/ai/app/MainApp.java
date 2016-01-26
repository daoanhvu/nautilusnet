package nautilus.ai.app;

import nautilus.ai.model.NautilusNet;
import nautilus.ai.ui.ImageForm;

import javax.swing.SwingUtilities;

import nautilus.ai.model.NNeuron;

public class MainApp {
	public static void main(String[] agrs) {
		
		
		SwingUtilities.invokeLater(new Runnable() {
            @Override
            public void run() {
            	ImageForm mainForm = new ImageForm();
            	mainForm.setVisible(true);
            }
        });
	}
	
	static void testANN() {
		/*
		Now we test a ANN with properties below:
			- learning rate: 0.5
			- Input: 2 + bias
			- Hidden: 2 + bias
			- output: 2
	*/
	NautilusNet aNet = new NautilusNet(0.5, 3, 3, 2);
	int i, j;
	NNeuron neuron;
	
	double[][] initW1 = {
						{0.15, 0.20, 0.35},
						{0.25, 0.30, 0.35}
						};
						
	double[][] initW2 = {
						{0.40, 0.45, 0.60},
						{0.50, 0.55, 0.60}
						};
	
	//Setup hidden layer		
	for(i=0; i<2; i++) {
		neuron = aNet.getHiddenLayer()[i];
		for(j=0; j<2; j++) {
			neuron.setWeight(initW1[i][j], j);
		}
		
		//set bias
		neuron.setWeight(initW1[1][2], 2);
	}
	aNet.getHiddenLayer()[2].setOutput(1.0);
	
	//Setup output layer
	for(i=0; i<2; i++) {
		neuron = aNet.getOutputLayer()[i];
		for(j=0; j<2; j++) {
			neuron.setWeight(initW2[i][j], j);
		}
		
		//set bias
		neuron.setWeight(initW2[1][2], 2);
	}
	
	//train the net
	double[] inputs = new double[] {0.05, 0.1, 1};
	double[] targets = new double[] {0.01, 0.99};
	
	aNet.setInputOutput(inputs, targets);
	
	aNet.forward();
	aNet.printNetwork(System.out);
	
	aNet.backward();
	aNet.printNetwork(System.out);
	}
}