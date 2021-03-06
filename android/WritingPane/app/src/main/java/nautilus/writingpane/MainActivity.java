package nautilus.writingpane;

import android.app.Application;
import android.content.Intent;
import android.graphics.Bitmap;
import android.graphics.Paint;
import android.os.Handler;
import android.os.Message;
import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;
import android.view.View;
import android.widget.Button;
import android.widget.CheckBox;
import android.widget.TextView;

import nautilus.ai.model.NautilusNet;
import nautilus.util.GraphUtilites;

public class MainActivity extends AppCompatActivity {

    private static final int MSG_LEARN_DONE = 0;
    private static final int MSG_RECOGNITE_DONE = 2;
    private static final int MSG_FAILED = 3;

    WritingPane writingPane;
    private TextView tvResult;
    CheckBox chkLearn;
    private Button btnClear;
    private Button btnProcess;
    Button btnOpenCamera;
    MyHandler mHandler;

    static class MyHandler extends Handler {
        final MainActivity mContext;
        MyHandler (MainActivity ctx) {
            super();
            mContext = ctx;
        }

        @Override
        public void handleMessage(Message msg) {
            Bitmap bmp;
            switch(msg.what) {
                case MSG_LEARN_DONE:
                    break;

                case MSG_RECOGNITE_DONE:
                    mContext.setResult((Result)msg.obj);
                    mContext.btnProcess.setEnabled(true);
                    break;

                case MSG_FAILED:
                    break;
            }
        }
    }

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        chkLearn = (CheckBox)findViewById(R.id.chkLearn);

        writingPane = (WritingPane)findViewById(R.id.writingPane);
        tvResult = (TextView)findViewById(R.id.tvResult);
        btnClear = (Button)findViewById(R.id.btnClear);
        btnClear.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                writingPane.clear();
            }
        });

        btnProcess = (Button)findViewById(R.id.btnProcess);
        btnProcess.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                final NautilusNet mTheNet = MyApplication.instance.getANN();
                btnProcess.setEnabled(false);
                if(chkLearn.isChecked()) {
                    //learn
                    Thread thread = new Thread(new Runnable() {
                        @Override
                        public void run() {
                            Paint p = new Paint();
                            p.setAntiAlias(true);
                            p.setStyle(Paint.Style.STROKE);
                            p.setStrokeJoin(Paint.Join.ROUND);
                            Message msg = new Message();
                            msg.what = MSG_LEARN_DONE;
                            Bitmap image = writingPane.getBitmap();
                            Bitmap image2 = GraphUtilites.lowpassFilter(image);
                            Bitmap image3 = GraphUtilites.fixBackWhiteImage(image2, p);
                            Bitmap image4 = GraphUtilites.resize(image3, MyApplication.SAMPLE_WIDTH, MyApplication.SAMPLE_HEIGHT);
                            image.recycle();
                            image2.recycle();
                            image3.recycle();
                            msg.obj = image4;
                            mHandler.sendMessage(msg);
                        }
                    });
                    thread.start();
                } else {
                    //recognize
                    Thread thread = new Thread(new Runnable() {
                        @Override
                        public void run() {
                            Paint p = new Paint();
                            Message msg = new Message();
                            p.setAntiAlias(true);
                            p.setStyle(Paint.Style.STROKE);
                            p.setStrokeJoin(Paint.Join.ROUND);
                            msg.what = MSG_RECOGNITE_DONE;
                            Bitmap image = writingPane.getBitmap();
                            Bitmap image2 = GraphUtilites.lowpassFilter(image);
                            Bitmap image3 = GraphUtilites.fixBackWhiteImage(image2, p);
                            Bitmap image4 = GraphUtilites.resize(image3, MyApplication.SAMPLE_WIDTH, MyApplication.SAMPLE_HEIGHT);
                            image.recycle();
                            image2.recycle();
                            image3.recycle();
                            GraphUtilites.getImageData(image4, data);
                            image4.recycle();
                            mTheNet.setInput(data);
                            mTheNet.forward();
                            int idx = mTheNet.getResultIndex();
                            result.character = MyApplication.getCharacter(idx);
                            msg.obj = result;
                            mHandler.sendMessage(msg);
                        }
                    });
                    thread.start();

//                    Intent intent = new Intent(MainActivity.this, CameraActivity.class);
//                    startActivity(intent);

                }
            }
        });

        btnOpenCamera = (Button)(Button)findViewById(R.id.btnOpenCamera);
        btnOpenCamera.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                Intent intent = new Intent(MainActivity.this, CameraActivity.class);
                startActivity(intent);
            }
        });
        mHandler = new MyHandler(this);
    }

    final double[] data = new double[MyApplication.SAMPLE_HEIGHT * MyApplication.SAMPLE_WIDTH];
    final Result result = new Result();

    void updateImage(Bitmap bmp) {
        writingPane.drawBitmap(bmp);
    }

    void setResult(Result result) {
        tvResult.setText("" + result.character);
    }

    static class Result {
        char character;
        double error;
    }
}
