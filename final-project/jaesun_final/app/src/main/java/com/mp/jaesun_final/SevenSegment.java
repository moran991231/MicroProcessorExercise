package com.mp.jaesun_final;

import android.util.Log;

public class SevenSegment {

    public static final byte[] GOOD__={'G','O','O','D','_','_'}, BAD___ ={'B','A','D','_','_','_'}, EMPTY={'_','_','_','_','_','_'};
    private int fd;
    private final String DRIVER_NAME= "/dev/sm9s5422_segment_js";

    public SevenSegment(){
        open();
    }

    public void open(){
        fd =  BoardIO.open(DRIVER_NAME, BoardIO.O_WRONLY);

        if(fd<0){
            Log.d("7-SEGMENT", "Failed to open 7-segment drive");
            return;
        }
    }
    public void close(){
        BoardIO.close(fd);
    }

    public void write(byte[]arr, int time){
        BoardIO.write(fd,arr,6,time);
    }
    public byte[] num2bytes(int num){
        byte[] n = new byte[6];

        for(int i=5; 0<=i; i--){
            n[i] = (byte)(num%10);
            num /=10;
        }
        return n;
    }

}
