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
	
	public static final int SAMPLE_WIDTH = 16;
	public static final int SAMPLE_HEIGHT = 24;
	private static final int INPUT_LENGTH = 384; // 24 * 32 NOT including bias
	private static final int HIDDEN_LENGTH = 268; //bias NOT included
//	public static final int OUTPUT_LENGTH = 62; // 26 lowercase characters + 26 upcase characters + 10 digits
	
	//for testing
	public static final int OUTPUT_LENGTH = 26;
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
	
	public double train(double[] inputs, double[] targets) {
		mBackproNet.setInputOutput(inputs, targets);
		mBackproNet.forward();		
		return mBackproNet.backward();
	}
	
	public int recognize(double[] inputs) {
		mBackproNet.setInput(inputs);
		mBackproNet.forward();
		return mBackproNet.getResultIndex();
	}
	
	public static void getTargetFromFolderName(char ch, double[] targets) {
		java.util.Arrays.fill(targets, 0.0);
		switch(ch) {
			case 'a':
				targets[0] = 1;
				break;
				
			case 'b':
				targets[1] = 1;
				break;
				
			case 'c':
				targets[2] = 1;
				break;
				
			case 'd':
				targets[3] = 1;
				break;
				
			case 'e':
				targets[4] = 1;
				break;
				
			case 'f':
				targets[5] = 1;
				break;
				
			case 'g':
				targets[6] = 1;
				break;
				
			case 'h':
				targets[7] = 1;
				break;
				
			case 'i':
				targets[8] = 1;
				break;
				
			case 'j':
				targets[9] = 1;
				break;
				
			case 'k':
				targets[10] = 1;
				break;
				
			case 'l':
				targets[11] = 1;
				break;
				
			case 'm':
				targets[12] = 1;
				break;
				
			case 'n':
				targets[13] = 1;
				break;
				
			case 'o':
				targets[14] = 1;
				break;
				
			case 'p':
				targets[15] = 1;
				break;
				
			case 'q':
				targets[16] = 1;
				break;
				
			case 'r':
				targets[17] = 1;
				break;
				
			case 's':
				targets[18] = 1;
				break;
				
			case 't':
				targets[19] = 1;
				break;
				
			case 'u':
				targets[20] = 1;
				break;
				
			case 'v':
				targets[21] = 1;
				break;
				
			case 'w':
				targets[22] = 1;
				break;
				
			case 'x':
				targets[23] = 1;
				break;
				
			case 'y':
				targets[24] = 1;
				break;
				
			case 'z':
				targets[25] = 1;
				break;
		}
	}
	
	public static char getCharacter(int index) {
		char result = 'a';
		switch(index) {
			case 0:
				result = 'a';
				break;
				
			case 1:
				result = 'b';
				break;
				
			case 2:
				result = 'c';
				break;
				
			case 3:
				result = 'd';
				break;
				
			case 4:
				result = 'e';
				break;
				
			case 5:
				result = 'f';
				break;
				
			case 6:
				result = 'g';
				break;
				
			case 7:
				result = 'h';
				break;
				
			case 8:
				result = 'i';
				break;
				
			case 9:
				result = 'j';
				break;
				
			case 10:
				result = 'k';
				break;
				
			case 11:
				result = 'l';
				break;
				
			case 12:
				result = 'm';
				break;
				
			case 13:
				result = 'n';
				break;
				
			case 14:
				result = 'o';
				break;
				
			case 15:
				result = 'p';
				break;
				
			case 16:
				result = 'q';
				break;
				
			case 17:
				result = 'r';
				break;
				
			case 18:
				result = 's';
				break;
				
			case 19:
				result = 't';
				break;
				
			case 20:
				result = 'u';
				break;
				
			case 21:
				result = 'v';
				break;
				
			case 22:
				result = 'w';
				break;
				
			case 23:
				result = 'x';
				break;
				
			case 24:
				result = 'y';
				break;
				
			case 25:
				result = 'z';
				break;
		}
		return result;
	}
}
