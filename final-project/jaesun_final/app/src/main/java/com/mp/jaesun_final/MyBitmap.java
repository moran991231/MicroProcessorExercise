package com.mp.jaesun_final;

import android.graphics.Bitmap;
import android.graphics.BitmapFactory;
import android.graphics.Matrix;
import android.util.Log;

import java.io.ByteArrayOutputStream;
import java.lang.reflect.Array;

public class MyBitmap {
    static {
        System.loadLibrary("image");
    }
    public native int rgb2hsv(Bitmap bitmap);
    public native int inRange(Bitmap bitmap, byte[] ranges);
//    public native void getRange(Bitmap bitmap, byte[] ranges);
    public static final int W=800, H=480;

    private final Matrix mtx_180 = new Matrix();

    private byte[] redRange, greenRange;

    public MyBitmap(){
        mtx_180.postRotate(180);
    }
    public Bitmap getImage(byte[] data){
        Bitmap img = BitmapFactory.decodeByteArray(data, 0, data.length);
        int w = img.getWidth(), h = img.getHeight();
        Log.d("BITMAP",String.format("orig captured image size: %d x %d",w,h));
        img = Bitmap.createBitmap(img, 0, 0, w, h, mtx_180, true);
        return img;
    }

    public Bitmap getCrop(Bitmap img){
        final int x=360, y=200+60, miniW=80, miniH=80;
        Bitmap crop = Bitmap.createBitmap(img,x,y,miniW, miniH);
        rgb2hsv(crop);
        return crop;
    }
    public byte[] getHsvRange(Bitmap img){ // cropped img
        final int MARGIN = 12;
        rgb2hsv(img);
        int[] ranges = new int[6];
        int w = img.getWidth(), h = img.getHeight();
        ByteArrayOutputStream stream = new ByteArrayOutputStream();
        img.compress(Bitmap.CompressFormat.PNG,100, stream);
        byte[] pixels= stream.toByteArray();
        stream=null;
        for(int i=0; i<w*h*4; i+=4){
            //h
            ranges[0] = Math.min(ranges[0], pixels[i+0]);
            ranges[1] = Math.max(ranges[1], pixels[i+0]);
            //s
            ranges[2] = Math.min(ranges[2], pixels[i+1]);
            ranges[3] = Math.max(ranges[3], pixels[i+1]);
            //v
            ranges[4] = Math.min(ranges[4], pixels[i+2]);
            ranges[5] = Math.max(ranges[5], pixels[i+2]);
        }

        ranges[0] = (ranges[0]-MARGIN<0)? ranges[0]-MARGIN+255: ranges[0]-MARGIN;
        ranges[1] = (ranges[1]+MARGIN>255)? ranges[1]+MARGIN-255:ranges[1]+MARGIN;

        ranges[2] = Math.max(ranges[2]-MARGIN, 0);
        ranges[3] = Math.min(ranges[3]+MARGIN, 255);

        ranges[4] = Math.max(ranges[4]-MARGIN, 0);
        ranges[5] = Math.min(ranges[5]+MARGIN, 255);

        byte[] ret = new byte[6];
        for(int i=0; i<6; i++)
            ret[i] = (byte) ranges[i];
        Log.d("BITMAP", arr2str(ranges));
        return ret;
    }

    public String arr2str(int[] arr){
        StringBuilder sb = new StringBuilder();
        sb.append('[');
        for(int x:arr)
            sb.append(x).append(' ');
        sb.append(']');
        return sb.toString();
    }
}
