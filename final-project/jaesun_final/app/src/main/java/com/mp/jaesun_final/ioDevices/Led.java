package com.mp.jaesun_final.ioDevices;

public class Led extends BoardIODevice {
    Led(String name, int opt) {
        super(name, opt);
        open();
    }

    @Override
    public void close() {
        writeStick(0);
        super.close();
    }

    public void writeStick(int stick) {
        stick = Math.max(stick, 0);
        stick = Math.min(stick, 8);
        byte[] data = new byte[8];
        for (int i = 0; i < stick; i++)
            data[7 - i] = 1;
        BoardIO.write(fd, data, 8, 1);
    }

    public void writeBin(int num) {
        byte[] data = new byte[8];
        for (int i = 0; i < 8; i++) {
            data[i] = (byte) ((num >> i) & 1);
        }
    }
}

