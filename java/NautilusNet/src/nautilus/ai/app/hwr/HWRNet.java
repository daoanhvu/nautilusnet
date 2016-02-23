package nautilus.ai.app.hwr;

import java.awt.image.BufferedImage;
import java.io.DataInputStream;
import java.io.DataOutputStream;
import java.io.File;
import java.io.FileInputStream;
import java.io.FileOutputStream;
import java.io.IOException;
import java.util.Random;

import javax.imageio.ImageIO;

import nautilus.ai.app.util.GraphicsUtil;
import nautilus.ai.model.NNeuron;
import nautilus.ai.model.NautilusNet;

/**
 * Hand writing recognizing network
 * @author Dao Anh Vu
 *
 */
public class HWRNet {
	
	public static final int SAMPLE_WIDTH = 54;
	public static final int SAMPLE_HEIGHT = 72;
	private static final int INPUT_LENGTH = 3888; // 54 * 72 NOT including bias
	private static final int HIDDEN_LENGTH = 1800; //bias NOT included
	private static final int OUTPUT_LENGTH = 62; // 26 lowercase characters + 26 upcase characters + 10 digits
	
	private NautilusNet mBackproNet;
	private double mLearningRate = 0.45;
	double[] bias = new double[2];
	
	public HWRNet() {
		buildBackproNet();
	}
	
	private void buildBackproNet() {
		mBackproNet = new NautilusNet(mLearningRate, 
				INPUT_LENGTH + 1, HIDDEN_LENGTH + 1, OUTPUT_LENGTH);
	}
	
	public void initializeWeight() {
		int i, j;
		Random generator = new Random();
		double w;
		NNeuron neuron;
		//initialize weights for hidden layer
		for(i=0; i<HIDDEN_LENGTH; i++) {
			neuron = mBackproNet.getHiddenLayer()[i];
			for(j=0; j<INPUT_LENGTH; j++) {
				w = generator.nextDouble();
				neuron.setWeight(w, j);
			}
			//Set bias for each row
			neuron.setWeight(bias[0], INPUT_LENGTH);
		}
		mBackproNet.getHiddenLayer()[HIDDEN_LENGTH].setOutput(1.0);
		
		//initialize weights for out layer
		for(i=0; i<OUTPUT_LENGTH; i++) {
			neuron = mBackproNet.getOutputLayer()[i];
			for(j=0; j<HIDDEN_LENGTH; j++) {
				w = generator.nextDouble();
				neuron.setWeight(w, j);
			}
			//Set bias for each row
			neuron.setWeight(bias[1], HIDDEN_LENGTH);
		}
	}
	
	public void readWeightFromFile(String filepath) {
		DataInputStream dis = null;
		int i, j;
		double w;
		NNeuron neuron;
		try {
			FileInputStream fis = new FileInputStream(new File(filepath));
			dis = new DataInputStream(fis);
			
			//Read biases
			bias[0] = dis.readDouble();
			bias[1] = dis.readDouble();
			
			//initialize weights for hidden layer
			for(i=0; i<HIDDEN_LENGTH; i++) {
				neuron = mBackproNet.getHiddenLayer()[i];
				for(j=0; j<INPUT_LENGTH; j++) {
					w = dis.readDouble();
					neuron.setWeight(w, j);
				}
				//Set bias for each row
				neuron.setWeight(bias[0], INPUT_LENGTH);
			}
			
			//initialize weights for output layer
			for(i=0; i<OUTPUT_LENGTH; i++) {
				neuron = mBackproNet.getOutputLayer()[i];
				for(j=0; j<HIDDEN_LENGTH; j++) {
					w = dis.readDouble();
					neuron.setWeight(w, j);
				}
				//Set bias for each row
				neuron.setWeight(bias[1], HIDDEN_LENGTH);
			}
		}catch(IOException ex) {
			ex.printStackTrace();
		} finally {
			try {
				if(dis != null)
					dis.close();
			}catch(IOException ex1) {
				ex1.printStackTrace();
			}
		}
	}
	
	public void writeWeight2File(String filepath) {
		DataOutputStream dos = null;
		NNeuron neuron;
		double w;
		int i, j;
		try {
			FileOutputStream fos = new FileOutputStream(new File(filepath));
			dos = new DataOutputStream(fos);
			
			//Write biases
			dos.writeDouble(bias[0]);
			dos.writeDouble(bias[1]);
			
			//initialize weights for hidden layer
			for(i=0; i<HIDDEN_LENGTH; i++) {
				neuron = mBackproNet.getHiddenLayer()[i];
				for(j=0; j<INPUT_LENGTH; j++) {
					w = neuron.getWeight(j);
					dos.writeDouble(w);
				}
			}
			
			//initialize weights for output layer
			for(i=0; i<OUTPUT_LENGTH; i++) {
				neuron = mBackproNet.getOutputLayer()[i];
				for(j=0; j<HIDDEN_LENGTH; j++) {
					w = neuron.getWeight(j);
					dos.writeDouble(w);
				}
			}
		}catch(IOException ex) {
			ex.printStackTrace();
		} finally {
			try {
				if(dos != null)
					dos.close();
			}catch(IOException ex1) {
				ex1.printStackTrace();
			}
		}
	}
	
	public void readSample(String file) {
		BufferedImage bImage;
		int i, j;
		int color;
		try {
			bImage = ImageIO.read(new File(file));
			int w = bImage.getWidth();
			int h = bImage.getHeight();
			
			float[] value = new float[w * h];
			float[] tmp = new float[4];
			
			for(i=0; i<h; i++) {
				for(j=0; j<w; j++) {
					color = bImage.getRGB(j, i);
					value[i * w + j] = GraphicsUtil.intRGBColor2Grayscale(color, tmp);
				}
			}
			
		} catch(IOException ex) {
			ex.printStackTrace();
		}
	}
	
	public void train(double[] inputs, double[] targets) {
		mBackproNet.setInputOutput(inputs, targets);
		
		mBackproNet.forward();
//		aNet.printNetwork(System.out);
		
		mBackproNet.backward();
//		aNet.printNetwork(System.out);
	}
	
	public double getTotalError() {
		return mBackproNet.getTotalError();
	}
	
}
