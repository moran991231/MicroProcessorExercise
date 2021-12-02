package com.mp.jaesun_final;

import android.util.Log;
import android.widget.Toast;

public class GpioButton {
    // fields
    private int fd=-1;
    private final String DRIVER_NAME = "/dev/sm9s5422_interrupt";
    public TranseThread mTranseThread;

    public static final int UP = 1, DOWN = 2, LEFT = 3, RIGHT = 4, CENTER = 5;
    public static final String[] directionCode = {"none", "UP", "DOWN", "LEFT", "RIGHT", "CENTER"};
    public int direction = 0;
    private boolean mConnectFlag = false;
    DoGpioButtonClicked afterClicked=null;

    public GpioButton(DoGpioButtonClicked content) {
        open();
        afterClicked=content;
    }

    // methods
    public void open() {
        if(fd>0) return;
        if (mConnectFlag) return;
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
        mTranseThread.interrupt();
        BoardIO.close(fd);
        fd=-1;
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
            while (mConnectFlag) {
                try {
                    direction = BoardIO.getInterrupt(fd);
                    String code = directionCode[direction];
                    Log.d("GPIO_BUTTON", "The button code is " + code+" id:"+this.getId());
                    if(mConnectFlag==false) return;
                    afterClicked.doThis(direction);
                    Thread.sleep(100);
                } catch (InterruptedException e) {
                    break;
                }
            }

            Log.d("GPIO_BUTTON", "Gpio Button thread ends");
        }
    }


}
