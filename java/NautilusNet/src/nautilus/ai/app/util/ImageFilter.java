package nautilus.ai.app.util;

import java.awt.Image;
import java.awt.image.BufferedImage;

/**
 * 
 * @author vdao5
 *
 * Please read http://northstar-www.dartmouth.edu/doc/idl/html_6.2/Filtering_an_Imagehvr.html#wp1022750
 */
public class ImageFilter {
	
	public static BufferedImage lowpassFilter(final BufferedImage inputImage) {
		int inputH = inputImage.getHeight();
		int inputW = inputImage.getWidth();
		int i, j;
		
		BufferedImage grayImage = new BufferedImage(inputW,inputH, BufferedImage.TYPE_BYTE_GRAY);
		int inColor, grayColor;
		int alpha, red, green, blue, gray;
		
		for(i=0; i<inputH; i++) {
			for(j=0; j<inputW; j++) {
				inColor = inputImage.getRGB(j, i);
				alpha = (inColor >> 24) & 0x000000ff;
				red = (inColor >> 16) & 0x000000ff;
				green = (inColor >> 8) & 0x000000ff;
				blue = inColor & 0x000000ff;
				gray = (red + green + blue)/3;
				grayImage.setRGB(j, i, gray);
			}
		}
		
		return grayImage;
	}
	
}
