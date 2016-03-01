package nautilus.writingpane;

import android.app.Application;

/**
 * Created by davu on 3/1/2016.
 */
public class MyApplication extends Application {

    public static MyApplication instance;

    @Override
    public void onCreate(){
        super.onCreate();
        instance = this;
    }

}
