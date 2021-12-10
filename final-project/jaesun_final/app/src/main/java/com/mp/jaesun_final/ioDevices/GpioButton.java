package com.mp.jaesun_final.ioDevices;

import android.util.Log;

public class GpioButton extends BoardIODevice {
    public static final int UP = 1, DOWN = 2, LEFT = 3, RIGHT = 4, CENTER = 5;
    public static final String[] directionCode = {"none", "UP", "DOWN", "LEFT", "RIGHT", "CENTER"};
    public int direction = 0;
    private ButtonThread btnThread;

    DoGpioButtonClicked afterClicked = null;

    public GpioButton(String name, int opt) {
        super(name, opt);
        open();
    }

    @Override
    public void open() {
        if (fd > 0) return;
        super.open();
        if (fd > 0) {
            btnThread = new ButtonThread();
            btnThread.start();
        } else fd = -1;
    }

    public void setAfterClick(DoGpioButtonClicked dothis) {
        afterClicked = dothis;
    }

    @Override
    public void close() {
        btnThread.isRunning = false;
        setAfterClick(null);
        btnThread.interrupt();
        super.close();
    }

    protected void finalize() throws Throwable {
        close();
        super.finalize();
    }

    private class ButtonThread extends Thread {
        public boolean isRunning = true;

        @Override
        public void run() {
            super.run();
            while (isRunning) {
                try {
                    direction = BoardIO.getInterrupt(fd);
                    String code = directionCode[direction];
                    Log.d("GPIO_BUTTON", "The button code is " + code + " id:" + this.getId());
                    if (isRunning == false) break;
                    if (afterClicked != null)
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
