package nautilus.util;

import java.nio.ByteBuffer;
import java.nio.ByteOrder;
import java.nio.FloatBuffer;
import java.nio.ShortBuffer;
import java.util.ArrayList;
import java.util.List;

import android.graphics.Bitmap;
import android.graphics.BitmapFactory;
import android.graphics.Canvas;
import android.graphics.Color;
import android.graphics.Paint;
import android.graphics.Rect;
import android.graphics.RectF;
import android.opengl.GLES20;
import android.util.Log;

public class GraphUtilites {
	
	/**
	 * @deprecated
	 * Need to be tested with OpenGL
	 * @param vcount The number of vertex
	 * @param rows each element in this array contains the number of vertex on the corresponding row
	 * @return
	 */
	public static short[] buildIndicesForTriangleStrip(int vcount, int[] rows){
		int i, j, num_of_vertices, nextCol, count, diff, k;
		
		List<Integer> indices = new ArrayList<Integer>();
		
		count = 0;
		for(i=0; i<rows.length-1; i++){
			num_of_vertices = rows[i];
			nextCol = rows[i+1];
		
			for(j=0; j<num_of_vertices; j++){
				indices.add(count);
				if(i>0 && j==0){ //first element of rows that not the first row
					indices.add(count);
				}
				
				if(j == num_of_vertices-1){
					if(i < rows.length - 2){
						if( (count + num_of_vertices) < vcount && j<nextCol){
							//neu co 1 phan tu ngay ben duoi
							indices.add(count + num_of_vertices);
							indices.add(count);
						}
						indices.add(count);
					} else	if( (count + num_of_vertices) < vcount && j<nextCol){
						k = count + num_of_vertices;
						while(k < vcount){
							indices.add(k);
							k++;
							if(k < vcount)
								indices.add(k);
							k++;
							if(k<vcount)
								indices.add(count);
						}
					}
				}else{
					//neu ngay ben duoc co mot vertex nua
					if( (count + num_of_vertices) < vcount && j<nextCol)
						indices.add(count + num_of_vertices);
					else{ //neu khong thi add vertex cuoi cung cua dong duoi
						diff = j - nextCol + 1;
						indices.add(count + num_of_vertices - diff);
					}
				}
				count++;
			}
		}
		
		short[] result = new short[indices.size()];
		int tmp;
		for(i=0; i<result.length; i++){
			tmp = indices.get(i);
			result[i] = (short)tmp;
		}
		
		return result;
	}
	
	public static short[] buildIndicesForTriangleStrip(int rows, int cols){
		// Now build the index data
		final int numStripsRequired = rows - 1;
		final int numDegensRequired = 2 * (numStripsRequired - 1);
		final int verticesPerStrip = 2 * cols;
		int offset = 0;
		short[] heightMapIndexData = new short[(verticesPerStrip * numStripsRequired)
		        + numDegensRequired];
		 
		for (int y = 0; y < rows - 1; y++) {
			if (y > 0) {
		        // Degenerate begin: repeat first vertex
				heightMapIndexData[offset++] = (short) (y * cols);
		    }
		 
		    for (int x = 0; x < cols; x++) {
		        // One part of the strip
		    	heightMapIndexData[offset++] = (short) ((y * cols) + x);
		        heightMapIndexData[offset++] = (short) (((y + 1) * cols) + x);
		    }
		 
		    if (y < rows - 2) {
		        // Degenerate end: repeat last vertex
		    	heightMapIndexData[offset++] = (short) (((y + 1) * cols) + (cols - 1));
		    }
		}
		
		return heightMapIndexData;
	}
	
	/**
	 * Just use this for quadratic vertex matrix
	 * @param vcount
	 * @param rows
	 * @return
	 */
	public static short[] buildIndicesForGLQUADs(int vcount, int[] rows){
		int i, j, colCount=0, k;
		List<Integer> indices = new ArrayList<Integer>();
		
		k = 0;
		for(i=0; i<rows.length-1; i++) {
			colCount = rows[i];
			for(j = 0; j<colCount; j++){
				indices.add(k);
				indices.add(k + colCount);
				indices.add(k + colCount + 1);
				indices.add(k + 1);
				k++;
			}
			k++;
		}
		
		short[] result = new short[indices.size()];
		int tmp;
		for(i=0; i<result.length; i++){
			tmp = indices.get(i);
			result[i] = (short)tmp;
		}
		
		return result;
	}
	
	/**
	 * Just use this for quadratic vertex matrix
	 * @param vcount
	 * @param rows
	 * @return
	 */
	public static short[] buildIndicesForGLLINEs(int vcount, int[] rows){
		int i, j, colCount=0, k;
		List<Integer> indices = new ArrayList<Integer>();
		
		k = 0;
		for(i=0; i<rows.length; i++) {
			colCount = rows[i];
			for(j = 0; j<colCount; j++) {
				indices.add(k);
				if(j>0 && j<colCount-1)
					indices.add(k);
				k++;
			}
		}
		
		colCount = rows[0];
		for(j = 0; j<colCount; j++) {
			for(i=0; i<rows.length; i++) {
				indices.add(j + colCount*i);
				if(i> 0 && i<rows.length-1)
					indices.add(j + colCount*i);
			}
		}
		
		short[] result = new short[indices.size()];
		int tmp;
		for(i=0; i<result.length; i++){
			tmp = indices.get(i);
			result[i] = (short)tmp;
		}
		
		return result;
	}
	
	/**
	 * angle = acos(v1 . v2);
	 * dot product between 2 vector (v1.v2) = v1.X* v2.X + v1.Y * v2.Y 
	 * @return
	 * 		angle in radian between v1 and v2
	 */
	public static float angle2DVector(float[] v1, float[] v2){
		double a1, a2;
		a1 = Math.atan2(v1[1], v1[0]);
		a2 = Math.atan2(v2[1], v2[0]);
		return (float)(a2 - a1);
	}
	
	/**
	 * angle = acos(v1 . v2);
	 * dot product between 2 vector (v1.v2) = v1.X* v2.X + v1.Y * v2.Y 
	 * @return
	 * 		angle in radian between v1 and v2
	 */
	public static float angle2DVector(float x1, float y1, float x2, float y2){
		double a1, a2;
		a1 = Math.atan2(y1, x1);
		a2 = Math.atan2(y2, x2);
		return (float)(a2 - a1);
	}

	/**
	 * @see http://www.rapidtables.com/convert/color/rgb-to-hsv.htm
	 * @see http://www.rapidtables.com/convert/color/hsv-to-rgb.htm
	 * @param inColor input color
	 * @param hsv output color in HSV space
	 */
	public static void grb2HSV(int inColor, float[] hsv) {
		int alpha = (inColor >> 24) & 0x000000ff;
		int red = (inColor >> 16) & 0x000000ff;
		int green = (inColor >> 8) & 0x000000ff;
		int blue = inColor & 0x000000ff;

		float r1 = red/255.0f;
		float g1 = green/255.0f;
		float b1 = blue/255.0f;
		float cmax = Math.max(Math.max(r1, g1), b1);
		float cmin = Math.min(Math.min(r1, g1), b1);
		float delta = cmax - cmin;

		if(delta == 0)
			hsv[0] = 0;
		else if(cmax == r1)
			hsv[0] = 60 * ((int)((g1 - b1)/delta) % 6);
		else if(cmax == g1) {
			hsv[0] = 60 * (((b1 - r1)/delta) + 2);
		} else {
			hsv[0] = 60 * (((r1 - g1)/delta) + 4);
		}

		hsv[1] = 0;
		if(cmax != 0) {
			hsv[1] = delta / cmax;
		}

		hsv[2] = cmax;
	}
	
	/**
	 * @see http://www.rapidtables.com/convert/color/hsv-to-rgb.htm
	 * @param hsv input color in HSV space, 0<=H<=360
	 * @return color in RGB space
	 */
	public static int grb2HSV(float[] hsv) {
		float C = hsv[1] * hsv[2];
		float X = C * (1 - Math.abs((((int) hsv[0]) / 60) % 2 - 1));
		float m = hsv[2] - C;
		
		int red, green, blue, alpha = 255;
	
		float r1 = 0;
		float g1 = 0;
		float b1 = 0;

		if( 0<= hsv[0] && hsv[0]<60 ) {
			r1 = C;
			g1 = X;
		} else if(60<=hsv[0] && hsv[0]<120) {
			r1 = X;
			g1 = C;
		} else if(120<=hsv[0] && hsv[0]<180) {
			g1 = C;
			b1 = X;
		} else if(180<=hsv[0] && hsv[0]<240) {
			g1 = X;
			b1 = C;
		} else if(240<=hsv[0] && hsv[0]<300) {
			r1 = X;
			b1 = C;
		} else {
			r1 = C;
			b1 = X;
		}
		
		red = (int)(r1 + m) * 255;
		green = (int)(g1 + m) * 255;
		blue = (int)(b1 + m) * 255;
		
		int color = ((alpha << 24) & 0xff000000) | ((red << 16) & 0x00ff0000) | ((green << 8) & 0x0000ff00)
				| (blue & 0x000000ff);
		
		return color;
	}
	
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
	
	public static int convertColorToInt(float[] inColor) {
		
		int red = (int)(inColor[0] * 255.0f);
		int green = (int)(inColor[1] * 255.0f);
		int blue = (int)(inColor[2] * 255.0f);
		int alpha = (int)(inColor[3] * 255.0f);
		
		int color = ((alpha << 24) & 0xff000000) | ((red << 16) & 0x00ff0000) | ((green << 8) & 0x0000ff00)
				| (blue & 0x000000ff);
		
		return color;
	}
	
	/**
	 * PixelXorXfermode is deprecated as of Android API 16 (Jelly Bean),
	 * and appears to be a no-op, so use this function instead
	*/
	public void invertColor(Canvas c, Bitmap b, int x, int y, int width, int height, int color) {
		int pixels[] = new int[width*height];
		int i;
		b.getPixels(pixels, 0, width, x, y, width, height);
		for (i = 0; i < width * height; i++) {
			pixels[i] ^= color;
		}
		c.drawBitmap(pixels, 0, width, x, y, width, height, /*hasAlpha=*/false, /*paint=*/null);
	}
	
	public static void findMinMax(float[] data, int dimension, float[] result) {
		int i;
		int vcount = data.length/dimension;
		float min = data[dimension-1];
		float max = data[dimension-1];
		
		for(i=2; i<=vcount; i++) {
			if(min > data[i*dimension-1])
				min = data[i*dimension-1];
			
			if(max < data[i*dimension-1])
				max = data[i*dimension-1];
		}
		
		result[0] = min;
		result[1] = max;
	}

	public static Bitmap resize(final Bitmap inputImage, int width, int height) {
		Bitmap bm = Bitmap.createScaledBitmap(inputImage, width, height, true);
		return bm;
	}

	public static Bitmap lowpassFilter(final Bitmap inputImage) {
		int inputH = inputImage.getHeight();
		int inputW = inputImage.getWidth();
		int y, j;

		Bitmap grayImage = Bitmap.createBitmap(inputW, inputH, Bitmap.Config.RGB_565);
		int inColor;
		int alpha, red, green, blue, gray;
		int color;
		for(y=0; y<inputH; y++) {
			for(j=0; j<inputW; j++) {
				inColor = inputImage .getPixel(j, y);
				alpha = (inColor >> 24) & 0x000000ff;
				red = (int) (((inColor >> 16) & 0x000000ff) * 0.299);
				green = (int) (((inColor >> 8) & 0x000000ff) * 0.578);
				blue = (int) ((inColor & 0x000000ff) * 0.114);
				gray = red + green + blue;
				if(gray > 180) {
					gray = 255;
				}
				color = Color.argb(alpha, gray, gray, gray);
				grayImage.setPixel (j, y, color);
			}
		}

		return grayImage;
	}

	public static void getImageData(final Bitmap inputImage,
										double[] result) {
		int inputH = inputImage.getHeight();
		int inputW = inputImage.getWidth();
		int y, x, c = 0;
		for(y=0; y<inputH; y++) {
			for(x=0; x<inputW; x++) {
				result[c++] = 1.0 - inputImage.getPixel(x, y)/255.0;
			}
		}
	}

	public static Bitmap fixBackWhiteImage(final Bitmap inputImage, final Paint paint) {
		int inputH = inputImage.getHeight();
		int inputW = inputImage.getWidth();
		int i, inColor;
		int left, top, right, bottom;
		int w, h;
		Bitmap outImage;
		Bitmap.Config config = inputImage.getConfig();
		boolean stop = false;
		top = 0;
		while( (top<inputH) && !stop ) {
			for(i=0; i<inputW; i++) {
				inColor = inputImage.getPixel(i, top);
				if(inColor != Color.WHITE) {
					stop = true;
					break;
				}
			}
			if(!stop)
				top++;
		}

		left = 0;
		stop = false;
		while((left<inputW) && !stop) {
			for(i=0; i<inputH; i++) {
				inColor = inputImage.getPixel(left, i);
				if(inColor != Color.WHITE) {
					stop = true;
					break;
				}
			}
			if(!stop)
				left++;
		}

		stop = false;
		bottom = inputH - 1;
		while( (bottom>=0) && !stop ) {
			for(i=0; i<inputW; i++) {
				inColor = inputImage.getPixel(i, bottom);
				if(inColor != Color.WHITE) {
					stop = true;
					break;
				}
			}
			if(!stop)
				bottom--;
		}

		stop = false;
		right = inputW - 1;
		while((right > 0) && !stop ) {
			for(i=0; i<inputH; i++) {
				inColor = inputImage.getPixel(right, i);
				if(inColor != Color.WHITE) {
					stop = true;
					break;
				}

			}
			if(!stop)
				right--;
		}
		h = bottom - top + 1;
		w = right - left + 1;
		outImage = Bitmap.createBitmap(w, h, config);
		Canvas g2 = new Canvas(outImage);
		Rect src = new Rect(left, top, left + w, top + h);
		RectF dst = new RectF(0, 0, w, h);
		g2.drawBitmap(inputImage, src, dst, paint);
		return outImage;
	}
	
	public static float[] generateColorByMinMax(float[] data, int vcount,
			int fromColor, int toColor, final float[] minmax) {
		int i;
		float[] colors = new float[vcount*4];
		//float mid = (minmax[0] + minmax[1])/2;
		float[] fromColor4f = {0, 0, 0, 0};
		float[] toColor4f = {0, 0, 0, 0};
		
		convertColorTo4f(fromColor, fromColor4f);
		convertColorTo4f(toColor, toColor4f);
		
		for(i=1; i<=vcount; i++) {
			//if( data[i*dimension - 1] > mid )
				System.arraycopy(fromColor4f, 0, colors, (i-1)*4, 4);
			//else 
				//System.arraycopy(toColor4f, 0, colors, (i-1)*4, 4);
		}
		
		return colors;
	}
	
	/**
	 * Create and compile a shader with specified type
	 * @param type type of shader (GLES20.GL_VERTEX_SHADER or GLES20.GL_FRAGMENT_SHADER)
	 * @param shaderCode String contains shader code
	 * @return id for the shader
	 */
	public static int loadShader(int type, String shaderCode){
		int shader = GLES20.glCreateShader(type);
		int[] compiled = { 0 };
		if(shader == 0) return 0;
			
		GLES20.glShaderSource(shader, shaderCode);
		GLES20.glCompileShader(shader);
		
		GLES20.glGetShaderiv(shader, GLES20.GL_COMPILE_STATUS, compiled, 0);
        if (compiled[0] == 0) {
            Log.e("GraphUtilities", GLES20.glGetShaderInfoLog(shader));
        }
        return shader;
	}
	
	/**
	 * 
	 * @param target one of value GLES20.GL_ARRAY_BUFFER or GLES20.GL_ELEMENT_ARRAY_BUFFER
	 */
	public static void initVertexBuffer(int target, float[] data, int bufferId) {
		int size = data.length * 4; //BYTES_PER_FLOAT;
		FloatBuffer fb = ByteBuffer.allocateDirect(size).order(ByteOrder.nativeOrder()).asFloatBuffer();
		fb.put(data);
		fb.position(0);
		
		GLES20.glBindBuffer(target, bufferId);
		GLES20.glBufferData(target, size, fb, GLES20.GL_STATIC_DRAW);
		GLES20.glBindBuffer(target, 0);
	}
	
	public static void initIndexBuffer(int target, short[] data, int bufferId) {
		int size = data.length * 2;//BYTES_PER_SHORT;
		ShortBuffer fb = ByteBuffer.allocateDirect(size).order(ByteOrder.nativeOrder()).asShortBuffer();
		fb.put(data);
		fb.position(0);
		
		GLES20.glBindBuffer(target, bufferId);
		GLES20.glBufferData(target, size, fb, GLES20.GL_STATIC_DRAW);
		GLES20.glBindBuffer(target, 0);
	}
}
