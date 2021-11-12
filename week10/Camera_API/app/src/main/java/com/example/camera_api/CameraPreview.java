package com.example.camera_api;

import android.view.SurfaceHolder;
import android.content.Context;
import android.hardware.Camera;
import android.util.Log;
import android.view.SurfaceView;
import java.io.IOException;
public class CameraPreview extends SurfaceView implements SurfaceHolder.Callback {

    private SurfaceHolder mHolder;
    private Camera mCamera;

    public CameraPreview(Context context, Camera camera){
        super(context);
        mCamera = camera;
        mHolder = getHolder();
        mHolder.addCallback(this);
    }
    @Override
    public void surfaceCreated(SurfaceHolder holder){
        try{
            if(mCamera != null){
                mCamera. setPreviewDisplay(holder)  ;
                mCamera.startPreview();
            }
        }catch(IOException e){
            Log.d(VIEW_LOG_TAG, "Error setting camera preview" + e.getMessage());
        }
    }

    @Override
    public void surfaceChanged(SurfaceHolder holder, int format, int w, int h){
        refreshCamera(mCamera);
    }
    @Override
    public void surfaceDestroyed(SurfaceHolder holder){
        if(mCamera==null) return;
        mCamera.stopPreview();
        mCamera=null;
    }
    public void refreshCamera(Camera camera){
        if(mHolder.getSurface() == null){
            return;
        }
        try{
            mCamera.stopPreview();
        }catch (Exception e){

        }
        setCamera(camera);
        try{
            mCamera.setPreviewDisplay(mHolder);
            mCamera.startPreview();
        }catch (Exception e){
            Log.d(VIEW_LOG_TAG, "Error starting camera preview "+ e.getMessage());
        }
    }
    public void setCamera(Camera camera){
        mCamera=camera;
    }
    public void resume()  {
        if(mCamera ==null) return;
        try{
            mCamera.setPreviewDisplay(mHolder)  ;
            mCamera.startPreview();
        }catch (IOException e){}

    }
    public void pause(){
        mCamera.stopPreview();
        mCamera=null;
    }
}
