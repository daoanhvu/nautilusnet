package nautilus.writingpane;

import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;
import android.view.View;
import android.widget.Button;
import android.widget.TextView;

public class MainActivity extends AppCompatActivity {

    private WritingPane writingPane;
    private TextView tvResult;
    private Button btnClear;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        writingPane = (WritingPane)findViewById(R.id.writingPane);
        tvResult = (TextView)findViewById(R.id.tvResult);
        btnClear = (Button)findViewById(R.id.btnClear);

        btnClear.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                writingPane.clear();
            }
        });
    }
}
