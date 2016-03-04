package nautilus.writingpane;

import android.app.Application;

import java.io.DataInputStream;
import java.io.DataOutputStream;
import java.io.File;
import java.io.FileInputStream;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.InputStream;

import nautilus.ai.model.NautilusNet;

/**
 * Created by davu on 3/1/2016.
 */
public class MyApplication extends Application {

    static final int SAMPLE_WIDTH = 16;
    static final int SAMPLE_HEIGHT = 24;
    static final int INPUT_LENGTH = 384; // 24 * 32 NOT including bias
    static final int HIDDEN_LENGTH = 268; //bias NOT included
    static final int OUTPUT_LENGTH = 26;

    private static final String NET_DATA_FILE = "nann.dat";
    public static MyApplication instance;

    //
    private NautilusNet mNetwork;

    @Override
    public void onCreate(){
        super.onCreate();
        instance = this;

        initANN();
    }

    /**
     * long task should be called in a thread other than the main thread
     */
    private void initANN() {
        File dir = getFilesDir();
        File datafile = new File(dir, NET_DATA_FILE);
        DataInputStream dis = null;

        mNetwork = new NautilusNet(0.75, INPUT_LENGTH, HIDDEN_LENGTH, OUTPUT_LENGTH);

        try {
            if(!datafile.exists()) {
                InputStream instr = getAssets().open(NET_DATA_FILE);
                dis = new DataInputStream(instr);
                mNetwork.readWeightInputStream(dis);
            } else {
                dis = new DataInputStream(new FileInputStream(datafile));
                mNetwork.readWeightInputStream(dis);
            }

        }catch(IOException ex) {
            ex.printStackTrace();
        } finally {
            try {
//                if(dos != null) {
//                    dos.close();
//                }

                if(dis != null) {
                    dis.close();
                }
            }catch(IOException ex2) {}
        }
    }
	
    public NautilusNet getANN() {
		return mNetwork;
	}

}
