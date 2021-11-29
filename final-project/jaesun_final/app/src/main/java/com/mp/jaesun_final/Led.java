package com.mp.jaesun_final;

import android.util.Log;

public class Led {
    // native library
    static {
        System.loadLibrary("led");
    }
    private native int openDriver(String path);
    private native void closeDriver();
    private native int writeDriver(byte[]data, int length);

    // field
    private final String DRIVER_NAME= "/dev/sm9s5422_led";

    public Led(){
        open();
    }
    public void open(){

        int ret =  openDriver(DRIVER_NAME);
        if(ret<0)
            Log.d("LED", "Failed to open LED driver");
    }
    public void close(){
        writeStick(0);
        closeDriver();
    }
    public void writeStick(int stick){
        stick = Math.max(stick,0);
        stick = Math.min(stick,8);
        byte[] data = new byte[8];
        for(int i=0; i<stick; i++)
            data[i]=1;
        writeDriver(data,8);
    }

    public void writeBin(int num){
        byte[]data = new byte[8];
        for(int i=0; i<8; i++){
            data[i] = (byte)((num>>i)&1);
        }
    }



}
