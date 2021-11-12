package com.example.buttoninterrupt;

import androidx.appcompat.app.AppCompatActivity;

import android.os.Bundle;
import android.os.Handler;
import android.os.Message;
import android.widget.TextView;
import android.widget.Toast;

import com.example.buttoninterrupt.databinding.ActivityMainBinding;

public class MainActivity extends AppCompatActivity implements IJniListener {

    TextView tv;
    JniDriver mDriver;
    boolean mThreadRun=true;
    private ActivityMainBinding binding;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);
        tv = (TextView) findViewById(R.id.textView1);
        mDriver = new JniDriver();
        mDriver.setListener(this);
        if(mDriver.open("/dev/sm9s5422_interrupt")<0){
            Toast.makeText(MainActivity.this,"Driver Open Failed", Toast.LENGTH_SHORT).show();
        }
    }

    @Override
    protected void onPause(){
        mDriver.close();
        super.onPause();
    }
    private final String[] strings = {"Up", "Down","Left","Right","Center"};
    public Handler handler = new Handler(){
        public void handleMessage(Message msg)  {
            if(1<=msg.arg1 && msg.arg1<=5)
                tv.setText(strings[msg.arg1-1]);
        }
    };
    @Override
    protected void onResume(){
        super.onResume();
        mDriver.open("/dev/sm9s5422_interrupt");
    }
    @Override
    public void onReceive(int val){
        Message text = Message.obtain();
        text.arg1=val;
        handler.sendMessage(text);
    }


}