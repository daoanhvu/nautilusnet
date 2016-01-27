package nautilus.ai.app.util;

public class GraphicsUtil {
	public static void convertColorTo3f(int inColor, float[] outColor) {
		int red = (inColor >> 16) & 0x000000ff;
		int green = (inColor >> 8) & 0x000000ff;
		int blue = inColor & 0x000000ff;
		
		outColor[0] = (red*1.0f) / 255;
		outColor[1] = (green*1.0f) / 255;
		outColor[2] = (blue*1.0f) / 255;
	}
	
	public static void convertColorTo4f(int inColor, float[] outColor) {
		int alpha = (inColor >> 24) & 0x000000ff;
		int red = (inColor >> 16) & 0x000000ff;
		int green = (inColor >> 8) & 0x000000ff;
		int blue = inColor & 0x000000ff;
		
		outColor[0] = red / 255.0f;
		outColor[1] = green / 255.0f;
		outColor[2] = blue / 255.0f;
		outColor[3] = alpha / 255.0f;
	}
	
	
	/**
	 * In this method, I used a formula (at https://en.wikipedia.org/wiki/Grayscale) 
	 * to convert RGB color to grayscale value
	 * @param inColor input color save as 4-bytes in
	 * @param tmp[] this is a byte array length >= 4, it's just used temporaly to prevent re-create the array
	 * 
	 * http://developer.bostjan-cigan.com/java-color-image-to-grayscale-conversion-algorithm/
	 * http://www.johndcook.com/blog/2009/08/24/algorithms-convert-color-grayscale/
	 * http://codesquire.com/post/GrayScaleJava
	 */
	public static float intRGBColor2Grayscale(int inColor, float[] tmp) {
		int alpha = (inColor >> 24) & 0x000000ff;
		int red = (inColor >> 16) & 0x000000ff;
		int green = (inColor >> 8) & 0x000000ff;
		int blue = inColor & 0x000000ff;
	
		tmp[0] = red / 255.0f;
		tmp[1] = green / 255.0f;
		tmp[2] = blue / 255.0f;
		tmp[3] = alpha / 255.0f;
		
		return (0.2126f * tmp[0] + 0.7152f * tmp[1] + 0.0722f * tmp[2]); 
	}
}
