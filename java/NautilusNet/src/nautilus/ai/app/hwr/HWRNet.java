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
	
	public static final int SAMPLE_WIDTH = 24;
	public static final int SAMPLE_HEIGHT = 32;
	private static final int INPUT_LENGTH = 768; // 24 * 32 NOT including bias
	private static final int HIDDEN_LENGTH = 624; //bias NOT included
//	public static final int OUTPUT_LENGTH = 62; // 26 lowercase characters + 26 upcase characters + 10 digits
	
	//for testing
	public static final int OUTPUT_LENGTH = 5;
	private NautilusNet mBackproNet;
	
	
	public HWRNet(double bias1, double bias2, double learningRate) {
		mBackproNet = new NautilusNet(learningRate, 
				INPUT_LENGTH, HIDDEN_LENGTH, OUTPUT_LENGTH);
		mBackproNet.setBiases(bias1, bias2);
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
				//w = generator.nextDouble();
				//trying to make the initialized weight smaller
				w = generator.nextDouble() / 20.0;
				neuron.setWeight(w, j);
			}
		}
		
		//initialize weights for out layer
		for(i=0; i<OUTPUT_LENGTH; i++) {
			neuron = mBackproNet.getOutputLayer()[i];
			for(j=0; j<HIDDEN_LENGTH; j++) {
				//w = generator.nextDouble();
				//trying to make the initialized weight smaller
				w = generator.nextDouble() / 20.0;
				neuron.setWeight(w, j);
			}
		}
	}
	
	public void readWeightFromFile(String filepath) {
		DataInputStream dis = null;
		int i, j;
		double w, bias1, bias2;
		NNeuron neuron;
		try {
			FileInputStream fis = new FileInputStream(new File(filepath));
			dis = new DataInputStream(fis);
			
			//Read biases
			bias1 = dis.readDouble();
			bias2 = dis.readDouble();
			mBackproNet.setBiases(bias1, bias2);
			
			//initialize weights for hidden layer
			for(i=0; i<HIDDEN_LENGTH; i++) {
				neuron = mBackproNet.getHiddenLayer()[i];
				for(j=0; j<INPUT_LENGTH; j++) {
					w = dis.readDouble();
					neuron.setWeight(w, j);
				}
			}
			
			//initialize weights for output layer
			for(i=0; i<OUTPUT_LENGTH; i++) {
				neuron = mBackproNet.getOutputLayer()[i];
				for(j=0; j<HIDDEN_LENGTH; j++) {
					w = dis.readDouble();
					neuron.setWeight(w, j);
				}
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
			dos.writeDouble(mBackproNet.getBias1());
			dos.writeDouble(mBackproNet.getBias2());
			
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
	
	public int recognize(double[] inputs) {
		mBackproNet.setInput(inputs);
		mBackproNet.forward();
		return mBackproNet.getResultIndex();
	}
	
	public double getTotalError() {
		return mBackproNet.getTotalError();
	}
	
	public double[] getErrors() {
		return mBackproNet.getErrors();
	}
}
