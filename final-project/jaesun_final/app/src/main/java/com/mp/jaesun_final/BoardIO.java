package com.mp.jaesun_final;

public class BoardIO {
    static {System.loadLibrary("board_io");}
    public static SevenSegment sevSeg;
    public static Led led;
    public static GpioButton gpioBtn;
    public static final int O_WRONLY = 1, O_RDONLY=0;
    public static native int open(String path,int option);
    public static native void close(int fd);
    public static native int getInterrupt(int fd);
    public static native void write(int fd, byte[]arr, int len, int time);
}
