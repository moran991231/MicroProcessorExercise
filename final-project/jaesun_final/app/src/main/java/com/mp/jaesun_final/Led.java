package com.mp.jaesun_final;

import android.util.Log;

public class Led {
    // field
    private int fd=-1;
    private final String DRIVER_NAME= "/dev/sm9s5422_led";

    public Led(){
        open();
    }
    public void open(){
        if(fd>0) return;
        fd  = BoardIO.open(DRIVER_NAME,BoardIO.O_WRONLY);
        if(fd<0)
            Log.d("LED", "Failed to open LED driver");
    }
    public void close(){
        if(fd>0) return;
        writeStick(0);
        BoardIO.close(fd);
        fd=-1;
    }
    public void writeStick(int stick){
        stick = Math.max(stick,0);
        stick = Math.min(stick,8);
        byte[] data = new byte[8];
        for(int i=0; i<stick; i++)
            data[7-i]=1;
        BoardIO.write(fd,data,8,1);
    }

    public void writeBin(int num){
        byte[]data = new byte[8];
        for(int i=0; i<8; i++){
            data[i] = (byte)((num>>i)&1);
        }
    }



}
