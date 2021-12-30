package com.mp.jaesun_final.ioDevices;

import android.util.Log;

abstract class BoardIODevice {
    protected int fd = -1;
    protected final String DRIVER_NAME;
    protected final int OPTION;

    BoardIODevice(String name, int opt) {
        DRIVER_NAME = name;
        OPTION = opt;
    }

    public void open() {
        if (fd > 0) return;
        fd = BoardIO.open(DRIVER_NAME, OPTION);
        if (fd < 0) {
            Log.d(DRIVER_NAME, "Failed to open driver.");
            return;
        }
    }

    public void close() {
        BoardIO.close(fd);
        fd = -1;
    }
}




