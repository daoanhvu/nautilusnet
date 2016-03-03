package nautilus.writingpane;

import android.app.Application;

import java.io.File;

import nautilus.ai.model.NautilusNet;

/**
 * Created by davu on 3/1/2016.
 */
public class MyApplication extends Application {

    public static MyApplication instance;

    //
    private NautilusNet mNetwork;

    @Override
    public void onCreate(){
        super.onCreate();
        instance = this;

        initANN();
    }

    private void initANN() {
        File dir = getFilesDir();


    }
	
		public NautilusNet getANN() {
		return mTheNet;
	}

}
