package com.mp.jaesun_final.ioDevices;

public class SevenSegment extends BoardIODevice {

    public static final byte[] GOOD__ = {'G', 'O', 'O', 'D', '_', '_'}, BAD___ = {'B', 'A', 'D', '_', '_', '_'}, EMPTY = {'_', '_', '_', '_', '_', '_'};

    public SevenSegment(String name, int opt) {
        super(name, opt);
        open();
    }

    public void write(byte[] arr, int time) {
        BoardIO.write(fd, arr, 6, time);
    }

    public byte[] num2bytes(int num) {
        byte[] n = new byte[6];

        for (int i = 5; 0 <= i; i--) {
            n[i] = (byte) (num % 10);
            num /= 10;
        }
        return n;
    }
}