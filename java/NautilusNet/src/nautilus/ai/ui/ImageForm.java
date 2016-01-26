package nautilus.ai.ui;

import java.awt.BorderLayout;
import java.awt.Container;
import java.awt.GridLayout;
import java.awt.Image;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;
import java.awt.event.WindowAdapter;
import java.awt.event.WindowEvent;
import java.awt.image.BufferedImage;
import java.io.File;
import java.util.concurrent.ExecutionException;

import javax.imageio.ImageIO;
import javax.swing.JButton;
import javax.swing.JFileChooser;
import javax.swing.JFrame;
import javax.swing.JLabel;
import javax.swing.JPanel;
import javax.swing.JTextField;
import javax.swing.SwingWorker;
import javax.swing.border.EmptyBorder;
import javax.swing.filechooser.FileFilter;

public class ImageForm extends JFrame {
	
	private static final long serialVersionUID = 1L;
	private JImagePanel mInputImagePane;
	private JImagePanel mTargetmagePane;
	private JPanel mButtonPane;
	private JButton mStartLearning;
	private JButton mRecognize;
	private JTextField mInputImagePath;
	private JButton mBrowse;
	
	public ImageForm() {
		super("Nautilus ANN Demo v1.0");
		iniComponent();
		initListeners();
	}
	
	private void iniComponent() {
		setBounds(20, 20, 500, 360);
		Container c = getContentPane();
		c.setLayout(new BorderLayout());
		
		//Input File Path pane
		JPanel northPane = new JPanel();
		mInputImagePath = new JTextField(20);
		mBrowse = new JButton("Browse");
		northPane.add(new JLabel("Input Image"));
		northPane.add(mInputImagePath);
		northPane.add(mBrowse);
		c.add(northPane, BorderLayout.NORTH);
		
		//Image pane
		GridLayout glayout = new GridLayout(1, 2, 10, 10);
		JPanel imagePane = new JPanel(glayout);
		mInputImagePane = new JImagePanel();
		mInputImagePane.setBorder(new EmptyBorder(10, 10, 10, 10));
		mTargetmagePane = new JImagePanel();
		imagePane.add(mInputImagePane);
		imagePane.add(mTargetmagePane);
		c.add(imagePane, BorderLayout.CENTER);
		
		//Button pane
		mButtonPane = new JPanel();
		
		mStartLearning = new JButton("Start");
		mButtonPane.add(mStartLearning);
		
		c.add(mButtonPane, BorderLayout.SOUTH);
	}
	
	private void initListeners() {
		this.addWindowListener(new WindowAdapter() {
			
			@Override
			public void windowClosing(WindowEvent e) {
				System.exit(0);
			}
			
		});
		
		mBrowse.addActionListener(new ActionListener() {
			@Override
			public void actionPerformed(ActionEvent e) {
				JFileChooser fc = new JFileChooser();
				fc.setAcceptAllFileFilterUsed(false);
				fc.setFileSelectionMode(JFileChooser.FILES_ONLY);
				fc.addChoosableFileFilter(new ImageFilter());
				int returnVal = fc.showOpenDialog(ImageForm.this);
				if (returnVal == JFileChooser.APPROVE_OPTION) {
		            File file = fc.getSelectedFile();
		            openInputImage(file);
		        } else {
		            //log.append("Open command cancelled by user." + newline);
		        }
			}
		});
	}
	
	private void openInputImage(final File file) {
		SwingWorker<Image, Void> worker = new SwingWorker<Image, Void>() {

			@Override
			protected Image doInBackground() throws Exception {
				BufferedImage bimage = null;
				try {
					bimage = ImageIO.read(file);
				} catch (Exception ex) {
					return null;
				}
				
				return bimage;
				
			}
			
			protected void done() {
				Image result;
				try {
					result = get();
					mInputImagePane.setImage(result);
				} catch (InterruptedException | ExecutionException e) {
					// TODO Auto-generated catch block
					e.printStackTrace();
				}
			}
		};
		worker.execute();
	}
	
	/* ImageFilter.java is used by FileChooserDemo2.java. */
	static class ImageFilter extends FileFilter {
		
		public final static String jpeg = "jpeg";
	    public final static String jpg = "jpg";
	    public final static String gif = "gif";
	    public final static String tiff = "tiff";
	    public final static String tif = "tif";
	    public final static String png = "png";
	 
	    //Accept all directories and all gif, jpg, tiff, or png files.
	    public boolean accept(File f) {
	        if (f.isDirectory()) {
	            return true;
	        }
	 
	        String extension = getExtension(f);
	        if (extension != null) {
	            if (extension.equals(tiff) ||
	                extension.equals(tif) ||
	                extension.equals(gif) ||
	                extension.equals(jpeg) ||
	                extension.equals(jpg) ||
	                extension.equals(png)) {
	                    return true;
	            } else {
	                return false;
	            }
	        }
	 
	        return false;
	    }
	 
	    //The description of this filter
	    public String getDescription() {
	        return "Just Images";
	    }
	    
	    public static String getExtension(File f) {
	        String ext = null;
	        String s = f.getName();
	        int i = s.lastIndexOf('.');

	        if (i > 0 &&  i < s.length() - 1) {
	            ext = s.substring(i+1).toLowerCase();
	        }
	        return ext;
	    }
	}
}
