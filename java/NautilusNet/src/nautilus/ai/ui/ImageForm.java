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
import javax.swing.JMenu;
import javax.swing.JMenuBar;
import javax.swing.JMenuItem;
import javax.swing.JPanel;
import javax.swing.JTextField;
import javax.swing.SwingUtilities;
import javax.swing.SwingWorker;
import javax.swing.border.Border;
import javax.swing.border.EtchedBorder;
import javax.swing.filechooser.FileFilter;

import nautilus.ai.app.util.ImageFilter;
import nautilus.ai.app.util.ImageOpenFilter;

public class ImageForm extends JFrame {
	
	private static final long serialVersionUID = 1L;
	private JImagePanel mInputImagePane;
	private JImagePanel mTargetmagePane;
	private JPanel mButtonPane;
	private JPanel mInfoPane;
	private JButton mStartLearning;
	private JButton mRecognize;
	private JTextField mInputImagePath;
	private JButton mBrowse;
	
	/* 
	 * I will use this in a inner class so I mask it as inner 
	 * attribute
	 **/
	JLabel mImageSizeLabel;
	
	/* Menu bar */
	private JMenuBar mMenuBar;
	private JMenu mMenuFile;
	private JMenu mMenuTool;
	private JMenu mMenuHelp;
	private JMenuItem mOpenLearningForm;
	private JMenuItem mQuit;
	
	private JCheckBox chkHighPassFilter;
	
	public ImageForm() {
		super("Nautilus ANN Demo v1.0");
		iniComponent();
		initListeners();
	}
	
	private void iniComponent() {
		setBounds(20, 20, 500, 360);
		Container c = getContentPane();
		c.setLayout(new BorderLayout());
		
		//Menubar
		mMenuBar = new JMenuBar();
		
		mMenuFile = new JMenu("File");
		mQuit = new JMenuItem("Quit");
		mQuit.addActionListener(new ActionListener() {
			@Override
			public void actionPerformed(ActionEvent e) {
				System.exit(0);
			}
		});
		mMenuFile.add(mQuit);
		
		mMenuTool = new JMenu("Tool");
		mOpenLearningForm = new JMenuItem("Train the network");
		mOpenLearningForm.addActionListener(new ActionListener() {
			@Override
			public void actionPerformed(ActionEvent e) {
				SwingUtilities.invokeLater(new Runnable() {
					@Override
					public void run() {
						TrainingForm form = new TrainingForm();
						form.setVisible(true);
					}
				});
			}
		});
		mMenuTool.add(mOpenLearningForm);
		
		mMenuHelp = new JMenu("Help");
		mMenuBar.add(mMenuFile);
		
		mMenuBar.add(mMenuTool);
		mMenuBar.add(mMenuHelp);
		
		
		setJMenuBar(mMenuBar);
		
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
		Border border = BorderFactory.createEtchedBorder(EtchedBorder.LOWERED);
		mInputImagePane.setBorder(border);
		imagePane.add(mInputImagePane);
		
		mTargetmagePane = new JImagePanel();
		border = BorderFactory.createEtchedBorder(EtchedBorder.LOWERED);
		mTargetmagePane.setBorder(border);
		imagePane.add(mTargetmagePane);
		c.add(imagePane, BorderLayout.CENTER);
		
		//Info pane
		glayout = new GridLayout(5, 1, 10, 10);
		mInfoPane = new JPanel(glayout);
		mInfoPane.setBounds(0, 0, 200, 400);
		border = BorderFactory.createEtchedBorder(EtchedBorder.LOWERED);
		mInfoPane.setBorder(border);
		JLabel lb = new JLabel("Image size:   ");
		mImageSizeLabel = new JLabel("N/A");
		mInfoPane.add(lb);
		mInfoPane.add(mImageSizeLabel);
		c.add(mInfoPane, BorderLayout.EAST);
		
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
				fc.addChoosableFileFilter(new ImageOpenFilter());
				//fc.setCurrentDirectory(new File("D:\\data\\nautilusnet"));
				fc.setCurrentDirectory(new File("D:\\data"));
				int returnVal = fc.showOpenDialog(ImageForm.this);
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
							bimage = mInputImagePane.getImage();
							result = ImageFilter.lowpassFilter(bimage);
							result = ImageFilter.fixImage(result);
							result = ImageFilter.resize2(result, 54, 72);
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
							mTargetmagePane.setImage(result);
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
					mInputImagePane.setImage(result);
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
