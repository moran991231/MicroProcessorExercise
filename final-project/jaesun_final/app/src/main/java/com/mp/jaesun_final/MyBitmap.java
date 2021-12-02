package com.mp.jaesun_final;

import android.graphics.Bitmap;
import android.graphics.BitmapFactory;
import android.graphics.Matrix;
import android.util.Log;

import java.io.ByteArrayOutputStream;
import java.lang.reflect.Array;
import java.nio.Buffer;
import java.nio.ByteBuffer;

public class MyBitmap {
    static {
        System.loadLibrary("image");
    }

    public static native int rgb2hsv(Bitmap bitmap);

    public static native int inRange(Bitmap bitmap, byte[] ranges);

    public static final int W = 800, H = 480;

    private static Matrix mtx_180;

    public static byte[] redRange = null, greenRange = null;

    public static boolean isCaliAvailable() {
        return redRange != null && greenRange != null;

    }

    public static Bitmap getImage(byte[] data) {
        if (mtx_180 == null) {
            mtx_180 = new Matrix();
            mtx_180.postRotate(180);
        }
        Bitmap img = BitmapFactory.decodeByteArray(data, 0, data.length);
        int w = img.getWidth(), h = img.getHeight();
        Log.d("BITMAP", String.format("orig captured image size: %d x %d", w, h));
        img = Bitmap.createBitmap(img, 0, 0, w, h, mtx_180, true);
        return img;
    }

    public static Bitmap getCrop(Bitmap img) {
        final int x = 360, y = 260, miniW = 80, miniH = 80;
        Bitmap crop = Bitmap.createBitmap(img, x, y, miniW, miniH);
        return crop;
    }

    public static byte[] getHsvRange(Bitmap img) { // cropped img
        final int MARGIN = 15;
        final int MASK = 0x00_00_00_FF;
        rgb2hsv(img);
        int[] ranges = {255, -255, 255, -255, 255, -255};
//        int[] hues = new int[255];
        int sum = 0;
        int w = img.getWidth(), h = img.getHeight();
        Log.d("BITMAP", String.format("cropped image size: %d x %d", w, h));
        ByteBuffer buffer = ByteBuffer.allocate(img.getByteCount());
        img.copyPixelsToBuffer(buffer);

        byte[] pixels = buffer.array();
        Log.d("BITMAP", String.format("pixels length: %d", pixels.length));

        for (int i = 0; i < w * h * 4; i += 4) {
            //h
//            hues[pixels[i+0]]++;
//            ranges[0] = Math.min(ranges[0], (int)pixels[i+0]&MASK);
//            ranges[1] = Math.max(ranges[1], (int)pixels[i+0]&MASK);
            sum += (int) pixels[i + 0] & MASK;
            //s
            ranges[2] = Math.min(ranges[2], (int) pixels[i + 1] & MASK);
            ranges[3] = Math.max(ranges[3], (int) pixels[i + 1] & MASK);
            //v
            ranges[4] = Math.min(ranges[4], (int) pixels[i + 2] & MASK);
            ranges[5] = Math.max(ranges[5], (int) pixels[i + 2] & MASK);
        }
        sum /= (w * h);
        ranges[0] = ranges[1] = sum;
        Log.d("BITMAP", "min max" + arr2str(ranges));
        ranges[0] = (ranges[0] - MARGIN < 0) ? ranges[0] - MARGIN + 255 : ranges[0] - MARGIN;
        ranges[1] = (ranges[1] + MARGIN > 255) ? ranges[1] + MARGIN - 255 : ranges[1] + MARGIN;

        ranges[2] = Math.max(ranges[2] - MARGIN, 0);
        ranges[3] = Math.min(ranges[3] + MARGIN, 255);

        ranges[4] = Math.max(ranges[4] - MARGIN, 0);
        ranges[5] = Math.min(ranges[5] + MARGIN, 255);

        byte[] ret = new byte[6];
        for (int i = 0; i < 6; i++)
            ret[i] = (byte) (ranges[i] & MASK);
        Log.d("BITMAP", arr2str(ret));
        return ret;
    }

    public static boolean isUp(Bitmap img) {
        int w = img.getWidth(), h = img.getHeight();
        ByteBuffer buffer = ByteBuffer.allocate(img.getByteCount());
        img.copyPixelsToBuffer(buffer);
        byte[] pixels = buffer.array();
        int i;
        int direction = 0;
        for (i = 0; i < w * (h / 2) * 4; i += 4) {
            if (pixels[i] != 0) direction++;
        }

        for (; i < w * h * 4; i += 4) {
            if (pixels[i] != 0) direction--;
        }
        return direction > 0;
    }
    public static String arr2str(int[] arr) {
        StringBuilder sb = new StringBuilder();
        sb.append('[');
        for (int x : arr)
            sb.append(x).append(' ');
        sb.append(']');
        return sb.toString();
    }

    public static String arr2str(byte[] arr) {
        StringBuilder sb = new StringBuilder();
        sb.append('[');
        for (byte x : arr)
            sb.append((int) x & 0xFF).append(' ');
        sb.append(']');
        return sb.toString();
    }

    public static int getMaxIndex(int[] arr) {
        int len = arr.length;
        int max = arr[0], max_i = 0;
        for (int i = 1; i < len; i++)
            if (arr[i] > max) {
                max = arr[i];
                max_i = i;
            }
        return max_i;
    }
}
