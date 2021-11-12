package com.example.segment;

import androidx.appcompat.app.AppCompatActivity;

import android.os.Bundle;
import android.widget.*;
import android.view.View;

import com.example.segment.databinding.ActivityMainBinding;


public class MainActivity extends AppCompatActivity {
    static {
        System.loadLibrary("segment_driver");
    }
    private native static int openDriver(String path);
    private native static void closeDriver();
    private native static void writeDriver(byte[] data, int length);

    int data_int;
    boolean mThreadRun, mStart;
    SegmentThread msegThread;
    
    private class SegmentThread extends Thread{
        @Override
        public void run(){
            super.run();
            while(mThreadRun){
                byte[] n = new byte[6];
                if(!mStart) writeDriver(n,n.length);
                else{
                    int data = data_int;
                    for(int i=5; 0<=i; i--){
                        n[i] = (byte)(data%10);
                        data /=10;
                    }
                    for(int i=0; i<50; i++)
                        writeDriver(n,n.length);
                    if(data_int>0) data_int--;
                }
            }
        }
    }

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);
        
        Button btn = (Button)findViewById((R.id.button1));
        btn.setOnClickListener(new View.OnClickListener(){
            @Override
            public void onClick(View view){
                String str = ((EditText)findViewById(R.id.editText1)).getText().toString();
                try{
                    data_int = Integer.parseInt(str);
                    mStart=true;
                }catch(NumberFormatException e){
                    data_int=0;
                    Toast.makeText(MainActivity.this, "Input Error", Toast.LENGTH_SHORT).show();
                }

            }
        });
    }
 
    @Override
    protected void onPause(){
        closeDriver();
        mThreadRun = false;
        msegThread = null;
        super.onPause();
    }
    @Override
    protected void onResume(){
        if(openDriver("/dev/sm9s5422_segment")<0){
            Toast.makeText(MainActivity.this,"Driver Open failed", Toast.LENGTH_SHORT).show();
        }
        mThreadRun = true;
        msegThread = new SegmentThread();
        msegThread.start();
        super.onResume();
    }
}

