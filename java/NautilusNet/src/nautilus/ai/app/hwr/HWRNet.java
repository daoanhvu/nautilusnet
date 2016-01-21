package nautilus.ai.app.hwr;

import java.awt.image.BufferedImage;
import java.io.DataOutputStream;
import java.io.File;
import java.io.FileOutputStream;
import java.io.IOException;

import javax.imageio.ImageIO;

import nautilus.ai.app.util.GraphicsUtil;
import nautilus.ai.model.NautilusNet;

/**
 * Hand writing recognizing network
 * @author Dao Anh Vu
 *
 */
public class HWRNet {
	
	private static final int SAMPLE_WIDTH = 45;
	private static final int SAMPLE_HEIGHT = 50;
	private static final int INPUT_LENGTH = 2251; // 45 * 50 + bias
	private static final int HIDDEN_LENGTH = 1801; //bias included
	private static final int OUTPUT_LENGTH = 62; // 26 lowcase characters + 26 upcase characters + 10 digits
	
	private NautilusNet mBackproNet;
	private double mLearningRate = 0.45;
	
	public HWRNet() {
		buildBackproNet();
	}
	
	private void buildBackproNet() {
		mBackproNet = new NautilusNet(mLearningRate, INPUT_LENGTH, HIDDEN_LENGTH, OUTPUT_LENGTH);
	}
	
	public void readWeightFromFile(String filepath) {
		
	}
	
	public void writeWeight2File(String filepath) {
		
	}
	
	public void readSample(String file) {
		BufferedImage bImage;
		int i, j;
		int color;
		try {
			bImage = ImageIO.read(new File(file));
			int w = bImage.getWidth();
			int h = bImage.getHeight();
			
			double[] value = new double[w * h];
			double[] tmp = new double[4];
			
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
	
	public void train(int loopCount) {
		int c = 0;
		
		while(c < loopCount) {
			
			c++;
		}
	}
	
}
