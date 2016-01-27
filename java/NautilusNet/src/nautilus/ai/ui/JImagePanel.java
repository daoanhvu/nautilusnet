package nautilus.ai.ui;

import java.awt.Graphics;
import java.awt.Image;

import javax.swing.JPanel;

public class JImagePanel extends JPanel {
	
	private static final long serialVersionUID = 1L;
	
	private Image mImage;
	
	public JImagePanel() {
		mImage = null;
	}
	
	public void setImage(Image img) {
		mImage = img;
		repaint();
	}
	
	public Image getImage() {
		return mImage;
	}
	
	@Override
	public void paintComponent(Graphics g) {
		int imageW, imageH;
		double imageAspect;
		int w = getWidth();
        int h = getHeight();
        double canvasAspect = (h * 1.0) / w;
        
        super.paintComponent(g);
		
		if(mImage != null) {
			imageW = mImage.getWidth(null);
			imageH = mImage.getHeight(null);
			imageAspect = (1.0 * imageH) / imageW;
			
			int x = 0, y = 0;
			
			if(imageAspect < canvasAspect) {
				y = h;
	            h = (int) (w * imageAspect);
	            y = (y - h) / 2; // center it along vertical
			} else {
				x = w;
	            w = (int) (h / imageAspect);
	            x = (x - w) / 2; // center it along horizontal
			}
			
	        g.drawImage(mImage, x, y, w + x, h + y, 0, 0, imageW, imageH, null);
		}
		
	}
	
}
