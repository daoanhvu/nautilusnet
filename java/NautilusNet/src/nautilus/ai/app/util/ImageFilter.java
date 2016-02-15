package nautilus.ai.app.util;

import java.awt.Color;
import java.awt.Image;
import java.awt.image.BufferedImage;

/**
 * 
 * @author vdao5
 *
 * Please read http://northstar-www.dartmouth.edu/doc/idl/html_6.2/Filtering_an_Imagehvr.html#wp1022750
 * http://students.iitk.ac.in/eclub/assets/tutorials/OPENCV%20TUTORIAL.pdf
 * http://www.tutorialspoint.com/java_dip/java_dip_tutorial.pdf
 * http://opencv-java-tutorials.readthedocs.org/en/latest/
 * http://ecee.colorado.edu/ecen5653/ecen5653/doc/opencv_tutorials.pdf
 * https://media.readthedocs.org/pdf/opencv-java-tutorials/latest/opencv-java-tutorials.pdf
 */
public class ImageFilter {
	
	public static BufferedImage getGrayscale(final BufferedImage inputImage) {
		int inputH = inputImage.getHeight();
		int inputW = inputImage.getWidth();
		int i, j;
		
		BufferedImage grayImage = new BufferedImage(inputW,inputH, BufferedImage.TYPE_BYTE_GRAY);
		int inColor, grayColor;
		int alpha, red, green, blue, gray;
		Color color;
		
		for(i=0; i<inputH; i++) {
			for(j=0; j<inputW; j++) {
				inColor = inputImage.getRGB(j, i);
				alpha = (inColor >> 24) & 0x000000ff;
				red = (int) (((inColor >> 16) & 0x000000ff) * 0.299);
				green = (int) (((inColor >> 8) & 0x000000ff) * 0.578);
				blue = (int) ((inColor & 0x000000ff) * 0.114);
				gray = red + green + blue;
				color = new Color(gray, gray, gray);
				
				grayImage.setRGB(j, i, color.getRGB());
			}
		}
		
		return grayImage;
	}
	
	public static BufferedImage lowpassFilter(final BufferedImage inputImage) {
		int inputH = inputImage.getHeight();
		int inputW = inputImage.getWidth();
		int i, j;
		
		BufferedImage grayImage = new BufferedImage(inputW,inputH, BufferedImage.TYPE_BYTE_GRAY);
		int inColor;
		int alpha, red, green, blue, gray;
		Color color;
		
		for(i=0; i<inputH; i++) {
			for(j=0; j<inputW; j++) {
				inColor = inputImage.getRGB(j, i);
				alpha = (inColor >> 24) & 0x000000ff;
				red = (int) (((inColor >> 16) & 0x000000ff) * 0.299);
				green = (int) (((inColor >> 8) & 0x000000ff) * 0.578);
				blue = (int) ((inColor & 0x000000ff) * 0.114);
				gray = red + green + blue;
				if(gray > 170) {
					gray = 255;
				}
				color = new Color(gray, gray, gray, alpha);
				grayImage.setRGB(j, i, color.getRGB());
			}
		}
		
		return grayImage;
	}
	
}
