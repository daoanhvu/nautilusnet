package nautilus.ai.ui;

import java.awt.BorderLayout;
import java.awt.Container;
import java.awt.Dimension;
import java.awt.GridLayout;
import java.awt.Image;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;
import java.awt.event.WindowAdapter;
import java.awt.event.WindowEvent;
import java.awt.image.BufferedImage;
import java.io.File;
import java.io.IOException;
import java.util.concurrent.ExecutionException;

import javax.imageio.ImageIO;
import javax.swing.BorderFactory;
import javax.swing.BoxLayout;
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
import javax.swing.SwingConstants;
import javax.swing.SwingUtilities;
import javax.swing.SwingWorker;
import javax.swing.border.BevelBorder;
import javax.swing.border.Border;
import javax.swing.border.EtchedBorder;
import javax.swing.filechooser.FileFilter;

import nautilus.ai.app.Application;
import nautilus.ai.app.hwr.HWRNet;
import nautilus.ai.app.util.ImageFilter;
import nautilus.ai.app.util.ImageOpenFilter;

import static nautilus.ai.app.Application.DATA_DIR;
import static nautilus.ai.app.Application.NETWORK_DATA_FILE;

public class ImageForm extends JFrame {
	
	private static final long serialVersionUID = 1L;
	private JImagePanel mInputImagePane;
	private JImagePanel mTargetmagePane;
	private JPanel mButtonPane;
	private JButton mStartLearning;
	private JButton btnRecognize, btnLoadTheNet;
	private JTextField mInputImagePath;
	private JButton mBrowse;
	
	HWRNet mTheNet;
	JLabel mImageSizeLabel;
	
	/* Menu bar */
	private JMenuBar mMenuBar;
	private JMenu mMenuFile;
	private JMenu mMenuTool;
	private JMenu mMenuHelp;
	private JMenuItem mOpenLearningForm;
	private JMenuItem mniPreprocessingForm;
	private JMenuItem mQuit;
	
	private JCheckBox chkHighPassFilter;
	
	public ImageForm() {
		super("Nautilus ANN Demo v1.0");
		iniComponent();
		initListeners();
		
		mTheNet = new HWRNet(.3, .5, .45);
	}
	
	private void iniComponent() {
		setBounds(20, 20, 800, 460);
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
		
		mniPreprocessingForm = new JMenuItem("Preprocess samples");
		mniPreprocessingForm.addActionListener(new ActionListener() {
			@Override
			public void actionPerformed(ActionEvent e) {
				SwingUtilities.invokeLater(new Runnable() {
					@Override
					public void run() {
						PreprocessingForm form = new PreprocessingForm();
						form.setVisible(true);
					}
				});
			}
		});
		mMenuTool.add(mniPreprocessingForm);
		
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
		JPanel mainPane = new JPanel(new BorderLayout());
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
		mainPane.add(imagePane, BorderLayout.CENTER);
		
		//Button pane
		mButtonPane = new JPanel();
		
		mStartLearning = new JButton("Test Process Image");
		mButtonPane.add(mStartLearning);
		
		btnLoadTheNet = new JButton("Load Network");
		mButtonPane.add(btnLoadTheNet);
		
		btnRecognize = new JButton("Recognize");
		mButtonPane.add(btnRecognize);
		mainPane.add(mButtonPane, BorderLayout.SOUTH);
		
		//Add main pane to frame
		c.add(mainPane, BorderLayout.CENTER);
		
		// create the status bar panel and shove it down the bottom of the frame
		JPanel statusPanel = new JPanel();
		statusPanel.setBorder(new BevelBorder(BevelBorder.LOWERED));
		c.add(statusPanel, BorderLayout.SOUTH);
		statusPanel.setPreferredSize(new Dimension(c.getWidth(), 22));
		statusPanel.setLayout(new BoxLayout(statusPanel, BoxLayout.X_AXIS));
		mImageSizeLabel = new JLabel("");
		mImageSizeLabel.setHorizontalAlignment(SwingConstants.LEFT);
		statusPanel.add(mImageSizeLabel);
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
				fc.setCurrentDirectory(new File(Application.getInstance().getStringValue(DATA_DIR)));
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
							result = ImageFilter.resize2(result, HWRNet.SAMPLE_WIDTH, HWRNet.SAMPLE_HEIGHT);
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
		
		btnLoadTheNet.addActionListener(new ActionListener() {
			@Override
			public void actionPerformed(ActionEvent e) {
				
				btnLoadTheNet.setEnabled(false);
				mImageSizeLabel.setText("Loading...");
				
				SwingWorker<BufferedImage, Void> worker = new SwingWorker<BufferedImage, Void>() {
					@Override
					protected BufferedImage doInBackground() throws Exception {
						mTheNet.readWeightFromFile(Application.getInstance().getStringValue(NETWORK_DATA_FILE));
						return null;
					}
					
					protected void done() {
						mImageSizeLabel.setText("Loading the net done!");
						btnLoadTheNet.setEnabled(true);
					}
				};
				worker.execute();
			}
		});
		
		btnRecognize.addActionListener(new ActionListener() {
			@Override
			public void actionPerformed(ActionEvent e) {
				SwingWorker<Integer, Void> worker = new SwingWorker<Integer, Void>() {

					@Override
					protected Integer doInBackground() throws Exception {
						double[] inputs = new double[HWRNet.SAMPLE_HEIGHT * HWRNet.SAMPLE_WIDTH];
						BufferedImage img = mTargetmagePane.getImage();
						ImageFilter.getImageData(img, inputs);
						int result = mTheNet.recognize(inputs);
						return result;
					}
					
					protected void done() {
						int result;
						try {
							String strResult = "";
							result = get();
							if(result==0)
								strResult = "a";
							else if(result==1)
								strResult = "b";
							else if(result==2)
								strResult = "c";
							else if(result==3)
								strResult = "d";
							else if(result==4)
								strResult = "e";
							
							mImageSizeLabel.setText("Recognizing done! Result: " + strResult);
							
							//for testing
							double[] errors = new double[HWRNet.OUTPUT_LENGTH];
							mTheNet.getErrors(errors);
							System.out.print("\n[");
							for(double e: errors) {
								System.out.print(e + ", ");
							}
							System.out.println("]");	
						} catch (InterruptedException e) {
							// TODO Auto-generated catch block
							e.printStackTrace();
						} catch (ExecutionException e) {
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
