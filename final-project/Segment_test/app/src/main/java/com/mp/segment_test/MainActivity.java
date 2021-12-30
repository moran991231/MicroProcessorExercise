package com.mp.segment_test;

import androidx.appcompat.app.AppCompatActivity;

import android.os.Bundle;
import android.util.Log;
import android.widget.Button;
import android.widget.TextView;

import com.mp.segment_test.databinding.ActivityMainBinding;

public class MainActivity extends AppCompatActivity {

    // Used to load the 'segment_test' library on application startup.
    static {
        System.loadLibrary("segment_test");
    }
    public static final int O_WRONLY = 1, O_RDONLY=0;
    public static native int open(String path,int option);
    public static native void close(int fd);
    public static native int getInterrupt(int fd);
    public static native void write(int fd, byte[]arr, int len, int time);

    private ActivityMainBinding binding;
    public static final byte[] GOOD__={'G','O','O','D','_','_'}, BAD___ ={'B','A','D','_','_','_'}, EMPTY={'_','_','_','_','_','_'};
    int fd;
    private final String DRIVER_NAME= "/dev/sm9s5422_segment_js";
    SegmentThread segThread= new SegmentThread();
    boolean mThreadRun=false;
    int a=0;
    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        fd =  open(DRIVER_NAME, O_WRONLY);
        if(fd<0){
            Log.d("7-SEGMENT", "Failed to open 7-segment drive");
            return;
        }
        mThreadRun=false;
        makeThreadRun();

        Button btn = (Button)  findViewById(R.id.button);
        btn.setOnClickListener(v->{
            if (a % 3 == 0)
                updateBuffer(GOOD__);
            else if (a % 3 == 1)
                updateBuffer(BAD___);
            else if (a % 3 == 2)
                updateBuffer(new byte[]{0, 1, 2, 3, 4, 5});
            a++;

        });


    }

    private byte[] buffer=EMPTY;
    private int updateCount=0;
    public void updateBuffer(byte[] in){
        buffer=in;
        updateCount++;
    }
    public void close(){
        makeThreadTerminated();
        close(fd);
    }
    public void makeThreadTerminated(){
        mThreadRun=false;
    }

    public void makeThreadRun(){
        if(mThreadRun) return;
        mThreadRun=true;
        segThread = new SegmentThread();
        segThread.start();
    }


    private class SegmentThread extends Thread{
        @Override
        public void run(){
            super.run();
            Log.d("7-SEGMENT", "begin");
            byte[] toWrite = null;
            int cnt=-1, tempCnt;
            while(mThreadRun){
                try {
                    toWrite=buffer;
                    tempCnt=updateCount;
                    if(tempCnt<=cnt){
                        Thread.sleep(100);
                        continue;
                    }
                    Log.d("7-SEGMENT", "msg print"+(char)toWrite[0]);
                    cnt = tempCnt;
                    toWrite=buffer.clone();

                    write(fd,toWrite,6,20);
                } catch (InterruptedException e) {
                    e.printStackTrace();
                }
            }
        }
    }


}