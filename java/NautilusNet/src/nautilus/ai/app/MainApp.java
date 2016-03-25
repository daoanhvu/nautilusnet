package nautilus.ai.app;

import nautilus.ai.model.NautilusNet;
import nautilus.ai.ui.ImageForm;

import static nautilus.ai.app.Application.ERROR_DATA_FILE;

import java.io.DataOutputStream;
import java.io.File;
import java.io.FileWriter;
import java.io.IOException;
import java.util.Random;

import javax.imageio.ImageIO;
import javax.swing.SwingUtilities;

import nautilus.ai.app.hwr.HWRNet;
import nautilus.ai.app.util.ImageFilter;
import nautilus.ai.app.util.ImageOpenFilter;
import nautilus.ai.model.NNeuron;

public class MainApp {
	
	public static void main(String[] agrs) {
		//Load configurations
		Application.getInstance();
		
//		System.out.println("Result: " + ( 0.1 + 0.2));
	
		runUIApp();
//		moveData();
//		testANN();
//		testANN2();
//		testAND();
	}
	
	static void moveData() {
		int l;
		byte[] buf = new byte[64];
		DataOutputStream dos = null;
		java.io.FileInputStream fis = null;
		try {
			dos = new DataOutputStream(new java.io.FileOutputStream(new File("D:/projects/NautilusNet/data/nautilusnet.dat")));
			fis = new java.io.FileInputStream(new File("D:/projects/NautilusNet/data/nautilusnet2.net"));
			
			//Write learning rate
			dos.writeDouble(0.75);
			
			while( (l=fis.read(buf)) > 0 ) {
				dos.write(buf, 0, l);
			}
			
			dos.flush();
		}catch(java.io.IOException ex) {
			ex.printStackTrace();
		} finally {
			try {
				if(dos != null) dos.close();
				if(fis != null) fis.close();
				System.out.println("Writing data done!!!!");
			}catch(java.io.IOException ex1) {
				ex1.printStackTrace();
			}
		}
	}
	
	static void runUIApp() {
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
	
	static void testANN2() {
		
		final int SAMPLE_WIDTH = 16;
		final int SAMPLE_HEIGHT = 24;
		final int INPUT_LENGTH = 384; // 24 * 32 NOT including bias
		final int HIDDEN_LENGTH = 268; //bias NOT included
		final int OUTPUT_LENGTH = 26;
		
		final ImageOpenFilter mImageFilter = new ImageOpenFilter();
		
		NautilusNet mBackproNet;
		int i, j;
		NNeuron neuron;
		double w;
		
		mBackproNet = new NautilusNet(0.45, INPUT_LENGTH, HIDDEN_LENGTH, OUTPUT_LENGTH);
		mBackproNet.setBiases(.65, .60);
		
		//initialize weights for hidden layer
		Random generator = new Random();
		for(i=0; i<HIDDEN_LENGTH; i++) {
			neuron = mBackproNet.getHiddenLayer()[i];
			for(j=0; j<INPUT_LENGTH; j++) {
//				w = generator.nextDouble();
				//trying to make the initialized weight smaller
				w = generator.nextDouble() / 100.0;
				neuron.setWeight(w, j);
			}
		}
				
		//initialize weights for out layer
		for(i=0; i<OUTPUT_LENGTH; i++) {
			neuron = mBackproNet.getOutputLayer()[i];
			for(j=0; j<HIDDEN_LENGTH; j++) {
//				w = generator.nextDouble();
				//trying to make the initialized weight smaller
				w = generator.nextDouble() / 100.0;
				neuron.setWeight(w, j);
			}
		}
		
		//train the net
		double[] inputs = new double[SAMPLE_HEIGHT * SAMPLE_WIDTH];
		double[] targets = new double[OUTPUT_LENGTH];
		
		java.awt.image.BufferedImage img;
		File folder = new File("D:/projects/NautilusNet/data/output_samples2/test1");
		File[] images = folder.listFiles(mImageFilter);
		double error;
		try {
			FileWriter fw = new FileWriter(Application.getInstance().getStringValue(ERROR_DATA_FILE));
			char ch;
			for(i=0; i<200; i++) {
				for(File f: images) {
					img = ImageIO.read(f);
					ch = f.getName().charAt(0);
					java.util.Arrays.fill(targets, 0.0);
					switch(ch) {
						case 'a':
							targets[0] = 1.0;
							break;
						case 'b':
							targets[1] = 1.0;
							break;
						case 'c':
							targets[2] = 1.0;
							break;
						case 'd':
							targets[3] = 1.0;
							break;
						case 'e':
							targets[4] = 1.0;
							break;
						case 'f':
							targets[5] = 1.0;
							break;
						case 'g':
							targets[6] = 1.0;
							break;
						case 'h':
							targets[7] = 1.0;
							break;
							
						case 'i':
							targets[8] = 1.0;
							break;
							
						case 'j':
							targets[9] = 1.0;
							break;
							
						case 'k':
							targets[10] = 1.0;
							break;
							
						case 'l':
							targets[11] = 1.0;
							break;
							
						case 'm':
							targets[12] = 1.0;
							break;
							
						case 'n':
							targets[13] = 1.0;
							break;
							
						case 'o':
							targets[14] = 1.0;
							break;
							
						case 'p':
							targets[15] = 1.0;
							break;
							
						case 'q':
							targets[16] = 1.0;
							break;
							
						case 'r':
							targets[17] = 1.0;
							break;
							
						case 's':
							targets[18] = 1.0;
							break;
							
						case 't':
							targets[19] = 1.0;
							break;
							
						case 'u':
							targets[20] = 1.0;
							break;
							
						case 'v':
							targets[21] = 1.0;
							break;
							
						case 'w':
							targets[22] = 1.0;
							break;
							
						case 'x':
							targets[23] = 1.0;
							break;
							
						case 'y':
							targets[24] = 1.0;
							break;
							
						case 'z':
							targets[25] = 1.0;
							break;
					}
					ImageFilter.getImageData(img, inputs);
					img.flush();
					mBackproNet.setInputOutput(inputs, targets);
					mBackproNet.forward();
					error = mBackproNet.backward();
					fw.write(error + "\n");
				}
			}
			fw.flush();
			fw.close();
			
			img = ImageIO.read(new File("D:/projects/NautilusNet/data/output_samples/b/2.png"));
			java.awt.image.BufferedImage img2 = ImageIO.read(new File("D:/projects/NautilusNet/data/output_samples/u/1.png"));
			ImageFilter.getImageData(img, inputs);
			mBackproNet.setInput(inputs);
			mBackproNet.forward();
			int result = mBackproNet.getResultIndex();
			System.out.println("Result: " + HWRNet.getCharacter(result));
			
			ImageFilter.getImageData(img2, inputs);
			mBackproNet.setInput(inputs);
			mBackproNet.forward();
			result = mBackproNet.getResultIndex();
			System.out.println("Result: " + HWRNet.getCharacter(result));
			
			//Save the network
			mBackproNet.writeWeight2File("D:/projects/NautilusNet/data/net_test.dat");
			
		}catch(IOException ex) {
			ex.printStackTrace();
		}
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
		
		inputs = new double[] {1, 1};
		targets = new double[] {1};
		System.out.println("++++++++++++++++++++++++++++++++++++++++++++ \n");
		aNet.setInputOutput(inputs, targets);
		aNet.forward();
		aNet.backward();
		
	}
}