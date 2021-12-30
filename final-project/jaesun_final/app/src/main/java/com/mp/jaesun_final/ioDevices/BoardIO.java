package com.mp.jaesun_final.ioDevices;

public class BoardIO {
    static {
        System.loadLibrary("board_io");
    }

    public static final int O_WRONLY = 1, O_RDONLY = 0;

    public static native int open(String path, int option);

    public static native void close(int fd);

    public static native int getInterrupt(int fd);

    public static native void write(int fd, byte[] arr, int len, int time);

    public static SevenSegment sevSeg = new SevenSegment("/dev/sm9s5422_segment_js", O_WRONLY);
    public static Led led = new Led("/dev/sm9s5422_led", O_WRONLY);
    public static GpioButton gpioBtn = null;
}
