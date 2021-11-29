package com.mp.jaesun_final;

import android.util.Log;

public class SevenSegment {
    static {
        System.loadLibrary("segment");
    }
    private native int openDriver(String path);
    private native void closeDriver();
    private native void writeDriver(byte[] arr, int count, int time);


    public static final byte[] GOOD__={71,'O','O','D','_','_'}, BAD___ ={'B','A','D','_','_','_'}, EMPTY={'_','_','_','_','_','_'};

    private byte[] buffer=EMPTY;
    private int updateCount=0;
    private final String DRIVER_NAME= "/dev/sm9s5422_segment_js";
    SegmentThread segThread= new SegmentThread();
    boolean mThreadRun=false;
    public SevenSegment(){
        open();
    }
    public void open(){
        int ret =  openDriver(DRIVER_NAME);
        if(ret<0){
            Log.d("7-SEGMENT", "Failed to open 7-segment drive");
            return;
        }
        mThreadRun=false;
        makeThreadRun();
    }
    public void close(){
        makeThreadTerminated();
        closeDriver();
    }
    public byte[] num2bytes(int num){
        byte[] n = new byte[6];

        for(int i=5; 0<=i; i--){
            n[i] = (byte)(num%10);
            num /=10;
        }
        return n;
    }

    public void updateBuffer(byte[] in){
        buffer=in;
        updateCount++;
    }

    public void makeThreadRun(){
        if(mThreadRun) return;
        mThreadRun=true;
        segThread = new SegmentThread();
        segThread.start();
    }
    public void makeThreadTerminated(){
        mThreadRun=false;
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

                    writeDriver(toWrite,6,20);

                } catch (InterruptedException e) {
                    e.printStackTrace();
                }
            }
        }
    }
}