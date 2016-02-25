package nautilus.ai.app;

import java.io.FileInputStream;
import java.io.IOException;
import java.io.InputStream;
import java.util.Properties;

public class Application {
	
	private static Application app = null;
	
	//Instance's members
	public static final int NO_DEBUG = 0;
	public static final int SIMPLE_STEP = 1;
	public static final int NETWORK_STEP = 2;
	
	public static final String DATA_DIR = "network_data_dir";
	public static final String INPUT_SAMPLE_DIR = "input_sample_dir";
	public static final String OUTPUT_SAMPLE_DIR = "output_sample_dir";
	public static final String NETWORK_DATA_FILE = "network_data_file";
	public static final String ERROR_DATA_FILE = "error_value_file";
	public static final String DEBUG_LEVEL = "debug_level";
	
	private Properties prop = new Properties();
	
	private Application() {
	}
	
	public static Application getInstance() {
		if(app == null) {
			app = new Application();
			app.loadConfiguration();
		}
		return app;
	}
	
	public int getDebugLevel() {
		String strLevel = prop.getProperty(DEBUG_LEVEL);
		return Integer.parseInt(strLevel);
	}
	
	public String getStringValue(String key) {
		return prop.getProperty(key);
	}
	
	public void loadConfiguration() {
		InputStream input = null;
		
		try {
			//input = new FileInputStream("nautilus.cfg");
			input = Thread.currentThread().getContextClassLoader().getResourceAsStream("nautilus.cfg");
			prop.load(input);
		}catch(IOException ex) {
			ex.printStackTrace();
		}finally {
			if (input != null) {
				try {
					input.close();
				} catch (IOException e) {
					e.printStackTrace();
				}
			}
		}
	}
	
}
