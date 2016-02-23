package nautilus.ai.ui;

import java.awt.BorderLayout;
import java.awt.Container;
import java.awt.Dimension;
import java.awt.GridBagLayout;
import java.awt.GridLayout;
import java.awt.Image;
import java.awt.Toolkit;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;
import java.awt.event.WindowAdapter;
import java.awt.event.WindowEvent;
import java.awt.image.BufferedImage;
import java.beans.PropertyChangeEvent;
import java.beans.PropertyChangeListener;
import java.io.File;
import java.io.FileFilter;
import java.io.IOException;
import java.util.concurrent.ExecutionException;

import javax.imageio.ImageIO;
import javax.swing.BorderFactory;
import javax.swing.BoxLayout;
import javax.swing.JButton;
import javax.swing.JCheckBox;
import javax.swing.JDialog;
import javax.swing.JFileChooser;
import javax.swing.JFrame;
import javax.swing.JLabel;
import javax.swing.JPanel;
import javax.swing.JProgressBar;
import javax.swing.JTextField;
import javax.swing.ProgressMonitor;
import javax.swing.SwingConstants;
import javax.swing.SwingWorker;
import javax.swing.border.BevelBorder;
import javax.swing.border.Border;
import javax.swing.border.EtchedBorder;

import nautilus.ai.app.hwr.HWRNet;
import nautilus.ai.app.util.ImageFilter;
import nautilus.ai.app.util.ImageOpenFilter;

public class PreprocessingForm extends JFrame {
	
	private static final long serialVersionUID = 10452L;
	private JPanel mButtonPane;
	private JPanel mInfoPane;
	private JButton mStartPreprocessing;
	private JButton mStartLearning;
	JTextField mImageFolderPath, mOuputFolderPath, mOutputNetFolder;
	private JButton mBrowse, mBrowse1, mNetOutputBrowse;
	JLabel mImageSizeLabel;
	File mSelectedDir;
	File mOutputDir;
	File mOutputNetFile;
	PreprocessingTask mPreprocessingTask;
	TrainingTask mTrainingTask;
	
	private JCheckBox chkPreprocessImage;
	private JCheckBox chkTrainTheNet;
	
	final ImageOpenFilter mImageFilter = new ImageOpenFilter();
	
	/* Progress bar dialo */
	JProgressBar progressBar;
	JDialog progressDialog;
	
	HWRNet mTheNet;
	
	
		
	private void initProgressDialog() {
		progressDialog = new JDialog(this);
		
		progressBar = new JProgressBar(0, 100);
		progressBar.setValue(0);
		progressBar.setStringPainted(true);
		
		progressDialog.add(progressBar);
	}
	
	
	public PreprocessingForm() {
		super("Train the net");
		iniComponent();
		initListeners();
		mTheNet = new HWRNet();
	}
	
	private void iniComponent() {
		setBounds(20, 20, 500, 360);
		Container c = getContentPane();
		c.setLayout(new BorderLayout());
		
		//Input File Path pane
		JPanel northPane = new JPanel(new GridLayout(2, 1));
		JPanel inputPane = new JPanel();
		mImageFolderPath = new JTextField(20);
		mBrowse = new JButton("Browse");
		inputPane.add(new JLabel("Input Image"));
		inputPane.add(mImageFolderPath);
		inputPane.add(mBrowse);
		
		mImageFolderPath.setText("D:\\projects\\NautilusNet\\data\\a");
		mSelectedDir = new File("D:\\projects\\NautilusNet\\data\\a");
		
		JPanel outputPane = new JPanel();
		mOuputFolderPath = new JTextField(20);
		mBrowse1 = new JButton("Browse");
		outputPane.add(new JLabel("Output folder"));
		outputPane.add(mOuputFolderPath);
		outputPane.add(mBrowse1);
		
		mOuputFolderPath.setText("D:\\projects\\NautilusNet\\data\\output\\a");
		mOutputDir = new File("D:\\projects\\NautilusNet\\data\\output\\a");
		
		northPane.add(inputPane);
		northPane.add(outputPane);
		c.add(northPane, BorderLayout.NORTH);
		
		
		GridBagLayout glayout = new GridBagLayout();
		mInfoPane = new JPanel(new BorderLayout());
		JPanel pnOutputNet = new JPanel();
		mOutputNetFolder = new JTextField(20);
		mNetOutputBrowse = new JButton("Browse");
		mOutputNetFolder.setText("D:\\projects\\NautilusNet\\data\\output\\nautilusnet.dat");
		mOutputNetFile = new File("D:\\projects\\NautilusNet\\data\\output\\nautilusnet.dat");
		pnOutputNet.add(new JLabel("Save the net: "));
		pnOutputNet.add(mOutputNetFolder);
		pnOutputNet.add(mNetOutputBrowse);
		mInfoPane.add(pnOutputNet, BorderLayout.SOUTH);
		c.add(mInfoPane, BorderLayout.CENTER);
		
		//Button pane
		mButtonPane = new JPanel();
		
		mStartLearning = new JButton("Start");
		mButtonPane.add(mStartLearning);
		
		//c.add(mButtonPane, BorderLayout.SOUTH);
		
		// create the status bar panel and shove it down the bottom of the frame
		JPanel statusPanel = new JPanel();
		statusPanel.setBorder(new BevelBorder(BevelBorder.LOWERED));
		c.add(statusPanel, BorderLayout.SOUTH);
		statusPanel.setPreferredSize(new Dimension(c.getWidth(), 16));
		statusPanel.setLayout(new BoxLayout(statusPanel, BoxLayout.X_AXIS));
		JLabel statusLabel = new JLabel("status");
		statusLabel.setHorizontalAlignment(SwingConstants.LEFT);
		statusPanel.add(statusLabel);
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
				fc.setCurrentDirectory(new File("D:\\projects\\NautilusNet\\data\\a"));
				int returnVal = fc.showOpenDialog(PreprocessingForm.this);
				if (returnVal == JFileChooser.APPROVE_OPTION) {
					mSelectedDir = fc.getSelectedFile();
					mImageFolderPath.setText(mSelectedDir.getPath());
		        }
			}
		});
		
		mBrowse1.addActionListener(new ActionListener() {
			@Override
			public void actionPerformed(ActionEvent e) {
				JFileChooser fc = new JFileChooser();
				fc.setAcceptAllFileFilterUsed(false);
				fc.setFileSelectionMode(JFileChooser.DIRECTORIES_ONLY);
				fc.addChoosableFileFilter(new ImageOpenFilter());
				//fc.setCurrentDirectory(new File("D:\\data\\nautilusnet"));
				fc.setCurrentDirectory(new File("D:\\projects\\NautilusNet\\data\\output\\a"));
				int returnVal = fc.showOpenDialog(PreprocessingForm.this);
				if (returnVal == JFileChooser.APPROVE_OPTION) {
					mOutputDir = fc.getSelectedFile();
					mOuputFolderPath.setText(mOutputDir.getPath());
		        }
			}
		});
		
		mNetOutputBrowse.addActionListener(new ActionListener() {
			@Override
			public void actionPerformed(ActionEvent e) {
				JFileChooser fc = new JFileChooser();
				fc.setAcceptAllFileFilterUsed(false);
//				fc.setFileSelectionMode(JFileChooser.DIRECTORIES_ONLY);
//				fc.addChoosableFileFilter(new ImageOpenFilter());
				//fc.setCurrentDirectory(new File("D:\\data\\nautilusnet"));
				fc.setCurrentDirectory(new File("D:\\projects\\NautilusNet\\data\\output"));
				int returnVal = fc.showOpenDialog(PreprocessingForm.this);
				if (returnVal == JFileChooser.APPROVE_OPTION) {
					mOutputNetFile = fc.getSelectedFile();
					mOutputNetFolder.setText(mOutputNetFile.getPath());
		        }
			}
		});
		
		mStartPreprocessing.addActionListener(new ActionListener() {
			@Override
			public void actionPerformed(ActionEvent e) {
				progressBar.setIndeterminate(true);
				mPreprocessingTask = new PreprocessingTask();
				mPreprocessingTask.addPropertyChangeListener(mProperChangeListener);
				mStartLearning.setEnabled(false);
				mPreprocessingTask.execute();
			}
		});
		
		mStartLearning.addActionListener(new ActionListener() {
			@Override
			public void actionPerformed(ActionEvent e) {
				progressBar.setIndeterminate(false);
				mPreprocessingTask = new PreprocessingTask();
				mPreprocessingTask.addPropertyChangeListener(mProperChangeListener);
				mStartLearning.setEnabled(false);
				mPreprocessingTask.execute();
			}
		});
	}
	
	PropertyChangeListener mProperChangeListener = new PropertyChangeListener() {
		
		@Override
		public void propertyChange(PropertyChangeEvent evt) {
			if("progress".equals(evt.getPropertyName())) {
				int progress = (Integer) evt.getNewValue();
				progressBar.setValue(progress);
			}
		}
	};
	
	class PreprocessingTask extends SwingWorker<Void, Void> {
		@Override
		public Void doInBackground() {
			int progress = 0;
			setProgress(0);
			BufferedImage bimage, result;
			File[] files = mSelectedDir.listFiles(mImageFilter);
			int total = files.length;
			int i, dotidx;
			float percent;
			File output;
			String filename;
			try {
				for(i=0; i<total; i++) {
					filename = files[i].getName();
					dotidx = filename.indexOf(".");
					if(dotidx >= 0) {
						filename = filename.substring(0, dotidx);
					}
					bimage = ImageIO.read(files[i]);
					result = ImageFilter.lowpassFilter(bimage);
					result = ImageFilter.fixImage(result);
					result = ImageFilter.resize2(result, 54, 72);
					output = new File(mOutputDir, filename + ".png");
					//Save the image to output folder
					ImageIO.write(result, "png", output);
					
					percent = ((i+1) * 100.0f) / total;
					progress = (int)percent;
					setProgress(progress);
				}
			} catch (IOException e) {
				// TODO Auto-generated catch block
				e.printStackTrace();
			}
			return null;
		}
		
		@Override
		public void done() {
//			Toolkit.getDefaultToolkit().beep();
			mStartPreprocessing.setEnabled(true);
		}
	}
	
	class TrainingTask extends SwingWorker<Void, Void> {
		@Override
		public Void doInBackground() {
			int progress = 0;
			setProgress(0);
			BufferedImage bimage, result;
			int total;
			int i, dotidx;
			float percent;
			File sampleDir = new File("D:\\projects\\NautilusNet\\data\\output");
			double[] inputs = new double[HWRNet.SAMPLE_HEIGHT * HWRNet.SAMPLE_WIDTH + 1];
			double[] targets = new double[4];
			try {
				
				File[] subfolders = sampleDir.listFiles(new FileFilter(){
					@Override
					public boolean accept(File f) {
						return f.isDirectory()?true:false;
					}
				});
				File[] images;
				BufferedImage img;
				
				//TODO: need to be improved
				total = 0;
				for(File folder: subfolders) {
					total += folder.listFiles(mImageFilter).length;
				}
				i = 0;
				for(File folder: subfolders) {
					String name = folder.getName();
					targets = getTargetFromFolderName(name);
					images = folder.listFiles(mImageFilter);
					for(File f: images) {
						img = ImageIO.read(f);
						ImageFilter.getImageData(img, inputs);
						inputs[inputs.length - 1] = 1.0;
						mTheNet.train(inputs, targets);
						percent = ((++i) * 100.0f) / total;
						progress = (int)percent;
						setProgress(progress);
					}
				}
			} catch (IOException e) {
				// TODO Auto-generated catch block
				e.printStackTrace();
			}
			return null;
		}
		
		@Override
		public void done() {
//			Toolkit.getDefaultToolkit().beep();
			mStartLearning.setEnabled(true);
		}
	}
	
	private double[] getTargetFromFolderName(String name) {
		double[] result = null;
		if(name.equals("a")) {
			result = new double[]{1, 0, 0, 0, 0};
		} else if(name.equals("b")) {
			result = new double[]{0, 1, 0, 0, 0};
		} else if(name.equals("c")) {
			result = new double[]{0, 0, 1, 0, 0};
		} else if(name.equals("d")) {
			result = new double[]{0, 0, 0, 1, 0};
		} else if(name.equals("e")) {
			result = new double[]{0, 0, 0, 0, 1};
		} else {
			//it's properly not here!!!!
			result = new double[]{0, 0, 0, 0, 0};
		}
		
		return result;
	}
	
	/**
	 * 
	 * @param dir
	 */
	private void trainTheNet(File dir) {
		double[] inputs = new double[HWRNet.SAMPLE_HEIGHT * HWRNet.SAMPLE_WIDTH + 1];
		double[] targets = new double[4];
		File[] subfolders = dir.listFiles(new FileFilter(){
			@Override
			public boolean accept(File f) {
				return f.isDirectory()?true:false;
			}
		});
		File[] images;
		BufferedImage img;
		try {
			for(File folder: subfolders) {
				String name = folder.getName();
				targets = getTargetFromFolderName(name);
				images = folder.listFiles(mImageFilter);
				for(File f: images) {
					img = ImageIO.read(f);
					ImageFilter.getImageData(img, inputs);
					inputs[inputs.length - 1] = 1.0;
					mTheNet.train(inputs, targets);
				}
			}
		}catch(IOException ex) {
			ex.printStackTrace();
		}
	}
}
