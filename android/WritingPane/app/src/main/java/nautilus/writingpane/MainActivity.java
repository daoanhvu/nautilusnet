package nautilus.writingpane;

import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;
import android.widget.TextView;

public class MainActivity extends AppCompatActivity {

    private WritingPane writingPane;
    private TextView tvResult;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        writingPane = (WritingPane)findViewById(R.id.writingPane);
        tvResult = (TextView)findViewById(R.id.tvResult);
    }
}
