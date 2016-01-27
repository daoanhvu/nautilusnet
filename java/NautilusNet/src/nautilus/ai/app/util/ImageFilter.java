package nautilus.ai.app.util;

import java.awt.Image;
import java.awt.image.BufferedImage;

import com.sun.prism.paint.Color;

/**
 * 
 * @author vdao5
 *
 * Please read http://northstar-www.dartmouth.edu/doc/idl/html_6.2/Filtering_an_Imagehvr.html#wp1022750
 */
public class ImageFilter {
	
	public static Image lowpassFilter(final BufferedImage inputImage) {
		int inputH = inputImage.getHeight();
		int inputW = inputImage.getWidth();
		int i, j;
		
		BufferedImage grayImage = new BufferedImage(inputH, inputW, BufferedImage.TYPE_BYTE_GRAY);
		float[][] grayValues = new float[inputH][inputW];
		float[] tmp = new float[4];
		int inColor, grayColor;
		int alpha, red, green, blue;
		
		for(i=0; i<inputH; i++) {
			for(j=0; j<inputW; j++) {
				inColor = inputImage.getRGB(j, i);
				alpha = (inColor >> 24) & 0x000000ff;
				red = (inColor >> 16) & 0x000000ff;
				green = (inColor >> 8) & 0x000000ff;
				blue = inColor & 0x000000ff;
				grayValues[i][j] = (0.2126f * tmp[0] + 0.7152f * tmp[1] + 0.0722f * tmp[2]);
				grayColor = (int) (grayValues[i][j] * 255.0f);
				grayImage.setRGB(j, i, grayColor);
			}
		}
		
		return grayImage;
	}
	
}
