package nautilus.ai.ui;

import static nautilus.ai.app.Application.OUTPUT_SAMPLE_DIR;
import static nautilus.ai.app.Application.NETWORK_DATA_FILE;
import static nautilus.ai.app.Application.ERROR_DATA_FILE;
import static nautilus.ai.app.Application.DEBUG_LEVEL;

import java.awt.BorderLayout;
import java.awt.Container;
import java.awt.Dimension;
import java.awt.GridBagConstraints;
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
import java.io.FileWriter;
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

import nautilus.ai.app.Application;
import nautilus.ai.app.hwr.HWRNet;
import nautilus.ai.app.util.ImageFilter;
import nautilus.ai.app.util.ImageOpenFilter;

public class TrainingForm extends JFrame implements PropertyChangeListener {
	
	private static final long serialVersionUID = 10452L;
	
	JTextField txtSampleDir;
	private JButton mSampleDirBrowse;
	JLabel lbStatusText;
	JButton btnLoadWeight, btnStartLearning, btnSaveTheNet;
	File mSampleDir;
	File mOutputNetFile;
	TrainingTask mTrainingTask;
	
	final ImageOpenFilter mImageFilter = new ImageOpenFilter();
	static final java.text.SimpleDateFormat TIME_FORMAT = new java.text.SimpleDateFormat("hh:mm:ss");
	
	/* Progress bar dialo */
	JProgressBar progressBar;
	
	HWRNet mTheNet;
	
	public TrainingForm() {
		super("Train the net");
		iniComponent();
		initListeners();
		mTheNet = new HWRNet(.75, .65, .45);
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
		
		txtSampleDir.setText(Application.getInstance().getStringValue(OUTPUT_SAMPLE_DIR));
		mSampleDir = new File(Application.getInstance().getStringValue(OUTPUT_SAMPLE_DIR));
//		txtSampleDir.setText("D:\\Documents\\testapp\\nautilusnet\\nautilusnet\\data\\output_samples");
//		mSampleDir = new File("D:\\Documents\\testapp\\nautilusnet\\nautilusnet\\data\\output_samples");
		
		//Button pane
		JPanel pnButtonPane = new JPanel();
		
		btnLoadWeight = new JButton("Load Weights");
		pnButtonPane.add(btnLoadWeight);
		
		btnStartLearning = new JButton("Start Training");
		pnButtonPane.add(btnStartLearning);
		btnSaveTheNet = new JButton("Save Weights");
		pnButtonPane.add(btnSaveTheNet);
		
		northPane.add(inputPane);
		northPane.add(pnButtonPane);
		c.add(northPane, BorderLayout.CENTER);
		
		GridBagLayout gbl = new GridBagLayout();
		JPanel statusBar = new JPanel(gbl);
		
		GridBagConstraints gbc = new GridBagConstraints();
		gbc.gridx = 0;
		gbc.gridy = 0;
		gbc.gridheight = 1;
		gbc.gridwidth = 1;
		gbc.weightx = 0.5;
		gbc.fill = GridBagConstraints.HORIZONTAL;
		lbStatusText = new JLabel();
		statusBar.add(lbStatusText, gbc);
		
		gbc = new GridBagConstraints();
		gbc.gridx = 1;
		gbc.gridy = 0;
		gbc.gridheight = 1;
		gbc.gridwidth = 3;
		gbc.weightx = 1.0;
		gbc.fill = GridBagConstraints.HORIZONTAL;
		progressBar = new JProgressBar(0, 100);
		progressBar.setValue(0);
		progressBar.setStringPainted(true);
		statusBar.add(progressBar, gbc);
		
		statusBar.setBorder(new BevelBorder(BevelBorder.LOWERED));
		statusBar.setPreferredSize(new Dimension(c.getWidth(), 16));
		c.add(statusBar, BorderLayout.SOUTH);
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
				fc.setFileSelectionMode(JFileChooser.DIRECTORIES_ONLY);
//				fc.addChoosableFileFilter(new ImageOpenFilter());
				mSampleDir = new File(txtSampleDir.getText());
				fc.setCurrentDirectory(mSampleDir);
				int returnVal = fc.showOpenDialog(TrainingForm.this);
				if (returnVal == JFileChooser.APPROVE_OPTION) {
					mSampleDir = fc.getSelectedFile();
					txtSampleDir.setText(mOutputNetFile.getPath());
		        }
			}
		});
		
		btnLoadWeight.addActionListener(new ActionListener() {
			@Override
			public void actionPerformed(ActionEvent e) {
				
				btnLoadWeight.setEnabled(false);
				
				SwingWorker<BufferedImage, Void> worker = new SwingWorker<BufferedImage, Void>() {
					@Override
					protected BufferedImage doInBackground() throws Exception {
//						mTheNet.readWeightFromFile("D:\\Documents\\testapp\\nautilusnet\\nautilusnet\\data\\nautilusnet.net");
						mTheNet.readWeightFromFile(Application.getInstance().getStringValue(NETWORK_DATA_FILE));
						return null;
					}
					
					protected void done() {
						btnLoadWeight.setEnabled(true);
					}
				};
				worker.execute();
			}
		});
		
		btnStartLearning.addActionListener(new ActionListener() {
			@Override
			public void actionPerformed(ActionEvent e) {
				progressBar.setIndeterminate(true);
				mTrainingTask = new TrainingTask();
				mTrainingTask.addPropertyChangeListener(TrainingForm.this);
				btnStartLearning.setEnabled(false);
				lbStatusText.setText("Start: " + TIME_FORMAT.format((new java.util.Date())));
				mTrainingTask.execute();
			}
		});
		
		btnSaveTheNet.addActionListener(new ActionListener() {
			@Override
			public void actionPerformed(ActionEvent e) {
				mTheNet.writeWeight2File(Application.getInstance().getStringValue(NETWORK_DATA_FILE));
			}
		});
	}
	
	class TrainingTask extends SwingWorker<Void, Void> {
		@Override
		public Void doInBackground() {
			int i, j, total, progress = 0;
			float percent;
			double error;
			double[] inputs = new double[HWRNet.SAMPLE_HEIGHT * HWRNet.SAMPLE_WIDTH];
			double[] targets = new double[HWRNet.OUTPUT_LENGTH];

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
				
				/* This is for debuging */
				FileWriter fw = new FileWriter(Application.getInstance().getStringValue(ERROR_DATA_FILE));
				
				//TODO: need to be improved
				total = subfolders.length * HWRNet.OUTPUT_LENGTH;
				int loop = 1600;
				String name;
				total = total * loop;
				i = 0;
				for(j=0; j< loop; j++) {
					for(File folder: subfolders) {
						images = folder.listFiles(mImageFilter);
						for(File f: images) {
							name = f.getName();
							HWRNet.getTargetFromFolderName(name.charAt(0), targets);
							img = ImageIO.read(f);
							ImageFilter.getImageData(img, inputs);
							error = mTheNet.train(inputs, targets);
							if( (Application.getInstance().getDebugLevel() == Application.SIMPLE_STEP) 
									&& (i%100 == 0)) {
								fw.write(error + "\n");
							}
							percent = ((++i) * 100.0f) / total;
							progress = (int)percent;
							setProgress(progress);
						}
					}
				}
				fw.close();
			} catch (IOException e) {
				// TODO Auto-generated catch block
				e.printStackTrace();
			}
			return null;
		}
		
		@Override
		public void done() {
//			Toolkit.getDefaultToolkit().beep();
			setProgress(100);
			btnStartLearning.setEnabled(true);
			String text = lbStatusText.getText();
			lbStatusText.setText(text + "   Finished at: " + TIME_FORMAT.format((new java.util.Date())));
		}
	}
	
	/**
	 * 
	 * @param dir
	 */
	private void trainTheNet(File dir) {
		double[] inputs = new double[HWRNet.SAMPLE_HEIGHT * HWRNet.SAMPLE_WIDTH];
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
				HWRNet.getTargetFromFolderName(name.charAt(0), targets);
				images = folder.listFiles(mImageFilter);
				for(File f: images) {
					img = ImageIO.read(f);
					ImageFilter.getImageData(img, inputs);
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
