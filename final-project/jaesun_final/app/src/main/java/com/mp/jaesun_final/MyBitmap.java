package com.mp.jaesun_final;

import android.graphics.Bitmap;

public class MyBitmap {
    static {
        System.loadLibrary("image");
    }
    public native int rgb2hsv(Bitmap bitmap);
    public native int inRange(Bitmap bitmap, byte[] ranges);
}
