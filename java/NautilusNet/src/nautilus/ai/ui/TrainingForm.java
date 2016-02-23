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

public class TrainingForm extends JFrame implements PropertyChangeListener {
	
	private static final long serialVersionUID = 10452L;
	
	JTextField txtSampleDir;
	private JButton mSampleDirBrowse;
	JButton btnStartLearning, btnSaveTheNet;
	JLabel mImageSizeLabel;
	File mSampleDir;
	File mOutputNetFile;
	TrainingTask mTrainingTask;
	
	final ImageOpenFilter mImageFilter = new ImageOpenFilter();
	
	/* Progress bar dialo */
	JProgressBar progressBar;
	
	HWRNet mTheNet;
	
	public TrainingForm() {
		super("Train the net");
		iniComponent();
		initListeners();
		mTheNet = new HWRNet();
		mTheNet.initializeWeight();
	}
	
	private void iniComponent() {
		setBounds(20, 20, 540, 160);
		Container c = getContentPane();
		c.setLayout(new BorderLayout());
		
		//Input File Path pane
		JPanel northPane = new JPanel(new GridLayout(3, 1));
		JPanel inputPane = new JPanel();
		txtSampleDir = new JTextField(30);
		mSampleDirBrowse = new JButton("Browse");
		inputPane.add(new JLabel("Sample Folder"));
		inputPane.add(txtSampleDir);
		inputPane.add(mSampleDirBrowse);
		
		txtSampleDir.setText("D:\\projects\\NautilusNet\\data\\output");
		mSampleDir = new File("D:\\projects\\NautilusNet\\data\\output");
		
		//Button pane
		JPanel pnButtonPane = new JPanel();
		btnStartLearning = new JButton("Start Training");
		pnButtonPane.add(btnStartLearning);
		btnSaveTheNet = new JButton("Save Weights");
		pnButtonPane.add(btnSaveTheNet);
		northPane.add(inputPane);
		northPane.add(pnButtonPane);
		
		progressBar = new JProgressBar(0, 100);
		progressBar.setValue(0);
		progressBar.setStringPainted(true);
		northPane.add(progressBar);
		
		c.add(northPane, BorderLayout.CENTER);
		
	}
	
	private void initListeners() {
		this.addWindowListener(new WindowAdapter() {
			@Override
			public void windowClosing(WindowEvent e) {
			}
		});
		
		mSampleDirBrowse.addActionListener(new ActionListener() {
			@Override
			public void actionPerformed(ActionEvent e) {
				JFileChooser fc = new JFileChooser();
				fc.setAcceptAllFileFilterUsed(false);
//				fc.setFileSelectionMode(JFileChooser.DIRECTORIES_ONLY);
//				fc.addChoosableFileFilter(new ImageOpenFilter());
				//fc.setCurrentDirectory(new File("D:\\data\\nautilusnet"));
				fc.setCurrentDirectory(new File("D:\\projects\\NautilusNet\\data\\output"));
				int returnVal = fc.showOpenDialog(TrainingForm.this);
				if (returnVal == JFileChooser.APPROVE_OPTION) {
					mSampleDir = fc.getSelectedFile();
					txtSampleDir.setText(mOutputNetFile.getPath());
		        }
			}
		});
		
		btnStartLearning.addActionListener(new ActionListener() {
			@Override
			public void actionPerformed(ActionEvent e) {
				progressBar.setIndeterminate(true);
				mTrainingTask = new TrainingTask();
				mTrainingTask.addPropertyChangeListener(TrainingForm.this);
				btnStartLearning.setEnabled(false);
				mTrainingTask.execute();
			}
		});
		
		btnSaveTheNet.addActionListener(new ActionListener() {
			@Override
			public void actionPerformed(ActionEvent e) {
				mTheNet.writeWeight2File("D:\\projects\\NautilusNet\\data\\output\\nautilusnet.net");
			}
		});
	}
	
	class TrainingTask extends SwingWorker<Void, Void> {
		@Override
		public Void doInBackground() {
			int i, total, progress = 0;
			float percent;
			double[] inputs = new double[HWRNet.SAMPLE_HEIGHT * HWRNet.SAMPLE_WIDTH + 1];
			double[] targets = new double[4];
			try {
				setProgress(0);
				File[] subfolders = mSampleDir.listFiles(new FileFilter(){
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
			btnStartLearning.setEnabled(true);
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

	@Override
	public void propertyChange(PropertyChangeEvent evt) {
		// TODO Auto-generated method stub
		if("progress".equals(evt.getPropertyName())) {
			int progress = (Integer) evt.getNewValue();
			progressBar.setIndeterminate(false);
	        progressBar.setValue(progress);
		}
	}
}
