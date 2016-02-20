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
import javax.swing.BorderFactory;
import javax.swing.JButton;
import javax.swing.JCheckBox;
import javax.swing.JFileChooser;
import javax.swing.JFrame;
import javax.swing.JLabel;
import javax.swing.JPanel;
import javax.swing.JTextField;
import javax.swing.SwingWorker;
import javax.swing.border.Border;
import javax.swing.border.EtchedBorder;

import nautilus.ai.app.util.ImageFilter;
import nautilus.ai.app.util.ImageOpenFilter;

public class TrainingForm extends JFrame {
	
	private static final long serialVersionUID = 10452L;
	private JPanel mButtonPane;
	private JPanel mInfoPane;
	private JButton mStartLearning;
	private JTextField mImageFolderPath;
	private JButton mBrowse;
	JLabel mImageSizeLabel;
	
	private JCheckBox chkHighPassFilter;
	
	public TrainingForm() {
		super("Train the net");
		iniComponent();
		initListeners();
	}
	
	private void iniComponent() {
		setBounds(20, 20, 500, 360);
		Container c = getContentPane();
		c.setLayout(new BorderLayout());
		
		//Input File Path pane
		JPanel northPane = new JPanel();
		mImageFolderPath = new JTextField(20);
		mBrowse = new JButton("Browse");
		northPane.add(new JLabel("Input Image"));
		northPane.add(mImageFolderPath);
		northPane.add(mBrowse);
		c.add(northPane, BorderLayout.NORTH);
		
		//Image pane
		GridLayout glayout = new GridLayout(1, 2, 10, 10);
		mInfoPane = new JPanel();
		c.add(mInfoPane, BorderLayout.CENTER);
		
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
			}
		});
		
		mBrowse.addActionListener(new ActionListener() {
			@Override
			public void actionPerformed(ActionEvent e) {
				JFileChooser fc = new JFileChooser();
				fc.setAcceptAllFileFilterUsed(false);
				fc.setFileSelectionMode(JFileChooser.DIRECTORIES_ONLY);
				fc.addChoosableFileFilter(new ImageOpenFilter());
				//fc.setCurrentDirectory(new File("D:\\data\\nautilusnet"));
				fc.setCurrentDirectory(new File("D:\\data"));
				int returnVal = fc.showOpenDialog(TrainingForm.this);
				if (returnVal == JFileChooser.APPROVE_OPTION) {
		            File file = fc.getSelectedFile();
		            openInputImage(file);
		        } else {
		            //log.append("Open command cancelled by user." + newline);
		        }
			}
		});
		
		mStartLearning.addActionListener(new ActionListener() {
			@Override
			public void actionPerformed(ActionEvent e) {
				SwingWorker<BufferedImage, Void> worker = new SwingWorker<BufferedImage, Void>() {

					@Override
					protected BufferedImage doInBackground() throws Exception {
						BufferedImage bimage = null;
						BufferedImage result = null;
						try {
							
						} catch (Exception ex) {
							ex.printStackTrace();
							return null;
						}
						
						return result;
					}
					
					protected void done() {
						BufferedImage result;
						try {
							result = get();
							
						} catch (InterruptedException | ExecutionException e) {
							// TODO Auto-generated catch block
							e.printStackTrace();
						}
					}
				};
				worker.execute();
			}
		});
	}
	
	private void openInputImage(final File file) {
		SwingWorker<BufferedImage, Void> worker = new SwingWorker<BufferedImage, Void>() {

			@Override
			protected BufferedImage doInBackground() throws Exception {
				BufferedImage bimage = null;
				try {
					bimage = ImageIO.read(file);
				} catch (Exception ex) {
					return null;
				}
				
				return bimage;
			}
			
			protected void done() {
				BufferedImage result;
				int width;
				int height;
				try {
					result = get();
					
					width = result.getWidth();
					height = result.getHeight();
					mImageSizeLabel.setText("W: " + width + "; H: " + height);
				} catch (InterruptedException | ExecutionException e) {
					// TODO Auto-generated catch block
					e.printStackTrace();
				}
			}
		};
		worker.execute();
	}
}
