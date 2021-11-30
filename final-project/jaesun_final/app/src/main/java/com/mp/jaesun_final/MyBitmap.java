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
        int[] ranges = {255,-255,255,-255,255,-255};
        int w = img.getWidth(), h = img.getHeight();
        Log.d("BITMAP",String.format("cropped image size: %d x %d",w,h));
        ByteBuffer buffer= ByteBuffer.allocate(img.getByteCount()); //바이트 버퍼를 이미지 사이즈 만큼 선언
        img.copyPixelsToBuffer(buffer);//비트맵의 픽셀을 버퍼에 저장

        byte[] pixels = buffer.array();
        Log.d("BITMAP",String.format("pixels length: %d",pixels.length));

        for(int i=0; i<w*h*4; i+=4){
            //h
            ranges[0] = Math.min(ranges[0], pixels[i+0]+128);
            ranges[1] = Math.max(ranges[1], pixels[i+0]+128);
            //s
            ranges[2] = Math.min(ranges[2], pixels[i+1]+128);
            ranges[3] = Math.max(ranges[3], pixels[i+1]+128);
            //v
            ranges[4] = Math.min(ranges[4], pixels[i+2]+128);
            ranges[5] = Math.max(ranges[5], pixels[i+2]+128);
        }
        Log.d("BITMAP","min max"+ arr2str(ranges));

        ranges[0] = (ranges[0]-MARGIN<0)? ranges[0]-MARGIN+255: ranges[0]-MARGIN;
        ranges[1] = (ranges[1]+MARGIN>255)? ranges[1]+MARGIN-255:ranges[1]+MARGIN;

        ranges[2] = Math.max(ranges[2]-MARGIN, 0);
        ranges[3] = Math.min(ranges[3]+MARGIN, 255);

        ranges[4] = Math.max(ranges[4]-MARGIN, 0);
        ranges[5] = Math.min(ranges[5]+MARGIN, 255);

        byte[] ret = new byte[6];
        for(int i=0; i<6; i++)
            ret[i] = (byte) (ranges[i]-128);
        Log.d("BITMAP", arr2str(ret));
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
    public String arr2str(byte[] arr){
        StringBuilder sb = new StringBuilder();
        sb.append('[');
        for(byte x:arr)
            sb.append(x).append(' ');
        sb.append(']');
        return sb.toString();
    }
}
