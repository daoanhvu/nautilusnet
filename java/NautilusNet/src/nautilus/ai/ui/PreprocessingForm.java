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

public class PreprocessingForm extends JFrame implements PropertyChangeListener {
	
	private static final long serialVersionUID = 10452L;
	private JPanel mInfoPane;
	private JButton btnStartPreprocessing;
	JTextField mImageFolderPath, mOuputFolderPath;
	JButton mBrowse, mBrowse1;
	JLabel mImageSizeLabel;
	File mSelectedDir;
	File mOutputDir;
	PreprocessingTask mPreprocessingTask;
	
	private JCheckBox chkPreprocessImage;
	private JCheckBox chkTrainTheNet;
	
	final ImageOpenFilter mImageFilter = new ImageOpenFilter();
	
	/* Progress bar dialo */
	JProgressBar progressBar;
	
	public PreprocessingForm() {
		super("Preprocessing samples ");
		iniComponent();
		initListeners();
	}
	
	private void iniComponent() {
		setBounds(20, 20, 540, 210);
		Container c = getContentPane();
		c.setLayout(new BorderLayout());
		
		//Input File Path pane
		JPanel northPane = new JPanel(new GridLayout(3, 1));
		JPanel inputPane = new JPanel();
		mImageFolderPath = new JTextField(30);
		mBrowse = new JButton("...");
		inputPane.add(new JLabel("Input Image"));
		inputPane.add(mImageFolderPath);
		inputPane.add(mBrowse);
		
//		mImageFolderPath.setText("D:\\projects\\NautilusNet\\data\\input_samples");
//		mSelectedDir = new File("D:\\projects\\NautilusNet\\data\\input_samples");
		mImageFolderPath.setText("D:\\Documents\\testapp\\nautilusnet\\nautilusnet\\data\\input_samples");
		mSelectedDir = new File("D:\\Documents\\testapp\\nautilusnet\\nautilusnet\\data\\input_samples");
		
		JPanel outputPane = new JPanel();
		mOuputFolderPath = new JTextField(30);
		mBrowse1 = new JButton("...");
		outputPane.add(new JLabel("Output folder"));
		outputPane.add(mOuputFolderPath);
		outputPane.add(mBrowse1);
		
//		mOuputFolderPath.setText("D:\\projects\\NautilusNet\\data\\output_samples");
//		mOutputDir = new File("D:\\projects\\NautilusNet\\data\\output_samples");
		mOuputFolderPath.setText("D:\\Documents\\testapp\\nautilusnet\\nautilusnet\\data\\output_samples");
		mOutputDir = new File("D:\\Documents\\testapp\\nautilusnet\\nautilusnet\\data\\output_samples");
		
		//Button pane
		JPanel pnButtonPane = new JPanel();
		btnStartPreprocessing = new JButton("Start");
		pnButtonPane.add(btnStartPreprocessing);
		
		northPane.add(inputPane);
		northPane.add(outputPane);
		northPane.add(pnButtonPane);
		c.add(northPane, BorderLayout.NORTH);
		
		GridBagLayout glayout = new GridBagLayout();
		mInfoPane = new JPanel(new BorderLayout());
		c.add(mInfoPane, BorderLayout.CENTER);
		
		progressBar = new JProgressBar(0, 100);
		progressBar.setValue(0);
		progressBar.setStringPainted(true);
		progressBar.setPreferredSize(new Dimension(c.getWidth(), 16));
		c.add(progressBar, BorderLayout.SOUTH);
		
		// create the status bar panel and shove it down the bottom of the frame
//		JPanel statusPanel = new JPanel();
//		statusPanel.setBorder(new BevelBorder(BevelBorder.LOWERED));
//		c.add(statusPanel, BorderLayout.SOUTH);
//		statusPanel.setPreferredSize(new Dimension(c.getWidth(), 16));
//		statusPanel.setLayout(new BoxLayout(statusPanel, BoxLayout.X_AXIS));
//		JLabel statusLabel = new JLabel("status");
//		statusLabel.setHorizontalAlignment(SwingConstants.LEFT);
//		statusPanel.add(statusLabel);
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
				mSelectedDir = new File(mImageFolderPath.getText());
				fc.setCurrentDirectory(mSelectedDir);
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
				mOutputDir = new File(mOuputFolderPath.getText());
				fc.setCurrentDirectory(mOutputDir);
				int returnVal = fc.showOpenDialog(PreprocessingForm.this);
				if (returnVal == JFileChooser.APPROVE_OPTION) {
					mOutputDir = fc.getSelectedFile();
					mOuputFolderPath.setText(mOutputDir.getPath());
		        }
			}
		});
		
		btnStartPreprocessing.addActionListener(new ActionListener() {
			@Override
			public void actionPerformed(ActionEvent e) {
				progressBar.setIndeterminate(true);
				mPreprocessingTask = new PreprocessingTask();
				mPreprocessingTask.addPropertyChangeListener(PreprocessingForm.this);
				btnStartPreprocessing.setEnabled(false);
				mPreprocessingTask.execute();
			}
		});
	}
		
	@Override
	public void propertyChange(PropertyChangeEvent evt) {
		if("progress".equals(evt.getPropertyName())) {
			int progress = (Integer) evt.getNewValue();
			progressBar.setIndeterminate(false);
	        progressBar.setValue(progress);
		}
	}
	
	
	class PreprocessingTask extends SwingWorker<Void, Void> {
		@Override
		public Void doInBackground() {
			int progress = 0;
			setProgress(0);
			BufferedImage bimage, result;
			File[] files;
			int total, k;
			int i, dotidx;
			float percent;
			File outFolder, outputFile;
			String filename;
			try {
				
				//get sub folders
				File[] subfolders = mSelectedDir.listFiles(new FileFilter(){
					@Override
					public boolean accept(File f) {
						return f.isDirectory()?true:false;
					}
				});
				
				//TODO: need to be improved
				total = 0;
				for(File folder: subfolders) {
					total += folder.listFiles(mImageFilter).length;
				}
				
				k = 0;
				for(File folder: subfolders) {
					files = folder.listFiles(mImageFilter);
					outFolder = new File(mOutputDir, folder.getName());
					if(!outFolder.exists())
						outFolder.mkdir();
					
					for(i=0; i<files.length; i++) {
						filename = files[i].getName();
						dotidx = filename.indexOf(".");
						if(dotidx >= 0) {
							filename = filename.substring(0, dotidx);
						}
						bimage = ImageIO.read(files[i]);
						result = ImageFilter.lowpassFilter(bimage);
						result = ImageFilter.fixImage(result);
						result = ImageFilter.resize2(result, HWRNet.SAMPLE_WIDTH, HWRNet.SAMPLE_HEIGHT);
						outputFile = new File(outFolder, filename + ".png");
						//Save the image to output folder
						ImageIO.write(result, "png", outputFile);
						
						percent = (++k * 100.0f) / total;
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
			btnStartPreprocessing.setEnabled(true);
		}
	}
}
