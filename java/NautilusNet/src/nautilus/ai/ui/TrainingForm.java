package nautilus.ai.ui;

import java.awt.BorderLayout;
import java.awt.Container;
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
import javax.swing.JButton;
import javax.swing.JCheckBox;
import javax.swing.JFileChooser;
import javax.swing.JFrame;
import javax.swing.JLabel;
import javax.swing.JPanel;
import javax.swing.JProgressBar;
import javax.swing.JTextField;
import javax.swing.ProgressMonitor;
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
	private JTextField mImageFolderPath, mOuputFolderPath;
	private JButton mBrowse, mBrowse1;
	JLabel mImageSizeLabel;
	File mSelectedDir;
	File mOutputDir;
	Task mTask;
	
	private JCheckBox chkHighPassFilter;
	
	final ImageOpenFilter mImageFilter = new ImageOpenFilter();
	ProgressMonitor progressMonitor;
	
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
		JPanel northPane = new JPanel(new GridLayout(2, 1));
		JPanel inputPane = new JPanel();
		mImageFolderPath = new JTextField(20);
		mBrowse = new JButton("Browse");
		inputPane.add(new JLabel("Input Image"));
		inputPane.add(mImageFolderPath);
		inputPane.add(mBrowse);
		
		JPanel outputPane = new JPanel();
		mOuputFolderPath = new JTextField(20);
		mBrowse1 = new JButton("Browse");
		outputPane.add(new JLabel("Output folder"));
		outputPane.add(mOuputFolderPath);
		outputPane.add(mBrowse1);
		northPane.add(inputPane);
		northPane.add(outputPane);
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
					mSelectedDir = fc.getSelectedFile();
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
				fc.setCurrentDirectory(new File("D:\\data"));
				int returnVal = fc.showOpenDialog(TrainingForm.this);
				if (returnVal == JFileChooser.APPROVE_OPTION) {
					mOutputDir = fc.getSelectedFile();
		        }
			}
		});
		
		mStartLearning.addActionListener(new ActionListener() {
			@Override
			public void actionPerformed(ActionEvent e) {
				progressMonitor = new ProgressMonitor(TrainingForm.this, 
						"Processing Images...", "", 0, 100);
				progressMonitor.setProgress(0);
				mTask = new Task();
				mTask.addPropertyChangeListener(mProperChangeListener);
				mStartLearning.setEnabled(false);
			}
		});
	}
	
	PropertyChangeListener mProperChangeListener = new PropertyChangeListener() {
		
		@Override
		public void propertyChange(PropertyChangeEvent evt) {
			if("progress".equals(evt.getPropertyName())) {
				int progress = (Integer) evt.getNewValue();
				progressMonitor.setProgress(progress);
	            String message =
	                String.format("Completed %d%%.\n", progress);
	            progressMonitor.setNote(message);
	            if (progressMonitor.isCanceled() || mTask.isDone()) {
	                Toolkit.getDefaultToolkit().beep();
	                if (progressMonitor.isCanceled()) {
	                	mTask.cancel(true);
	                }
	                mStartLearning.setEnabled(true);
	            }
			}
		}
	};
	
	class Task extends SwingWorker<Void, Void> {
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
			mStartLearning.setEnabled(true);
            progressMonitor.setProgress(100);
		}
	}
	
	private void preprocessImage(File dir) {
		File[] files = dir.listFiles(mImageFilter);
		for(File f: files) {
			
		}
	}
}
