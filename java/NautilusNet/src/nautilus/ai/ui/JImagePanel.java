package nautilus.ai.ui;

import java.awt.EventQueue;
import java.awt.Graphics;
import java.awt.Image;
import java.awt.image.BufferedImage;

import javax.swing.JPanel;

public class JImagePanel extends JPanel {
	
	private static final long serialVersionUID = 1L;
	
	private BufferedImage mImage;
	
	public JImagePanel() {
		mImage = null;
	}
	
	public void setImage(BufferedImage img) {
		mImage = img;
		EventQueue.invokeLater(new Runnable() {
			@Override
			public void run() {
				repaint();
			}
		});
	}
	
	public BufferedImage getImage() {
		return mImage;
	}
	
	@Override
	public void paintComponent(Graphics g) {
		int imageW, imageH;
		int x = 0, y = 0;
		double imageAspect;
		int w = getWidth();
        int h = getHeight();
        double canvasAspect = (h * 1.0) / w;
        
        super.paintComponent(g);
		
		if(mImage != null) {
			imageW = mImage.getWidth(null);
			imageH = mImage.getHeight(null);
			
			if( (imageW <= w) && (imageH <= h) ) {
				x = (w - imageW) / 2; // center it along horizontal
				y = (h - imageH) / 2; // center it along vertical
				g.drawImage(mImage, x, y, imageW, imageH, null);
			} else {
				imageAspect = (1.0 * imageH) / imageW;
				
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
	
}
