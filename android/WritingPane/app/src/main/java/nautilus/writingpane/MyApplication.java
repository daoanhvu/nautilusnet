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

    public static char getCharacter(int index) {
        char result = 'a';
        switch(index) {
            case 0:
                result = 'a';
                break;

            case 1:
                result = 'b';
                break;

            case 2:
                result = 'c';
                break;

            case 3:
                result = 'd';
                break;

            case 4:
                result = 'e';
                break;

            case 5:
                result = 'f';
                break;

            case 6:
                result = 'g';
                break;

            case 7:
                result = 'h';
                break;

            case 8:
                result = 'i';
                break;

            case 9:
                result = 'j';
                break;

            case 10:
                result = 'k';
                break;

            case 11:
                result = 'l';
                break;

            case 12:
                result = 'm';
                break;

            case 13:
                result = 'n';
                break;

            case 14:
                result = 'o';
                break;

            case 15:
                result = 'p';
                break;

            case 16:
                result = 'q';
                break;

            case 17:
                result = 'r';
                break;

            case 18:
                result = 's';
                break;

            case 19:
                result = 't';
                break;

            case 20:
                result = 'u';
                break;

            case 21:
                result = 'v';
                break;

            case 22:
                result = 'w';
                break;

            case 23:
                result = 'x';
                break;

            case 24:
                result = 'y';
                break;

            case 25:
                result = 'z';
                break;
        }
        return result;
    }
}
