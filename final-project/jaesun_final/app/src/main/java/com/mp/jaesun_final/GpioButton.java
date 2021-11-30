package com.mp.jaesun_final;

import android.util.Log;
import android.widget.Toast;

public class GpioButton {
    // native library
//    static {  System.loadLibrary("gpio_button"); }
//    public native int openDriver(String path);
//
//    public native void closeDriver();
//
//    public native int getInterrupt();

    // fields
    private int fd;
    private final String DRIVER_NAME = "/dev/sm9s5422_interrupt";
    public TranseThread mTranseThread;

    public static final int UP = 1, DOWN = 2, LEFT = 3, RIGHT = 4, CENTER = 5;
    public static final String[] directionCode = {"none", "UP", "DOWN", "LEFT", "RIGHT", "CENTER"};
    public int direction = 0;
    private boolean mConnectFlag = false;

    public GpioButton() {
        open();
    }

    // methods
    public void open() {
        if (mConnectFlag) return ;
        fd = BoardIO.open(DRIVER_NAME, BoardIO.O_RDONLY);
        if (fd > 0) {
            mConnectFlag = true;
            mTranseThread = new TranseThread();
            mTranseThread.start();
        } else {
            fd=-1;
        }
    }

    public void close() {
        mConnectFlag = false;
        BoardIO.close(fd);
    }

    protected void finalize() throws Throwable {
        close();
        super.finalize();
    }


    // nested class
    private class TranseThread extends Thread {
        @Override
        public void run() {
            super.run();
            try {
                while (mConnectFlag) {
                    try {
                        direction = BoardIO.getInterrupt(fd);
                        String code = directionCode[direction];
                        Log.d("GPIO_BUTTON", "The button code is " + code);
                        Thread.sleep(100);
                    } catch (InterruptedException e) {
                        e.printStackTrace();
                    }
                }
            } catch (Exception e) {
            }

            Log.d("GPIO_BUTTON", "Gpio Button thread ends");
        }
    }


}
