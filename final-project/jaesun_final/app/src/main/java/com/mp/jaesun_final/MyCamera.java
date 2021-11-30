package com.mp.jaesun_final;

import android.content.Context;
import android.hardware.Camera;
import android.util.Log;
import android.view.SurfaceHolder;
import android.view.SurfaceView;
import android.widget.FrameLayout;
import android.widget.ImageView;

import java.io.IOException;
import java.util.List;

public class MyCamera {

    private Camera camera;
    private CameraPreview preview;
    private FrameLayout imgView;

    public void open(MainActivity main) {
        if (camera != null) return;
        camera = getCameraInstance();
        camera.setDisplayOrientation(180);
        Camera.Parameters params = camera.getParameters();

       params.setPreviewSize(800,480);
        camera.setParameters(params);
        preview = new CameraPreview(main, camera);
        imgView = main.camPreview;
        imgView.addView(preview);

    }

    public void close() {
        if (camera == null) return;
        preview.pause();
        camera.release();
        camera = null;
    }

    private static Camera getCameraInstance() {
        try {
            return Camera.open();
        } catch (Exception e) {
            e.printStackTrace();
            return null;
        }
    }


}


class CameraPreview extends SurfaceView implements SurfaceHolder.Callback {

    private SurfaceHolder mHolder;
    private Camera cam;

    public CameraPreview(Context context, android.hardware.Camera camera) {
        super(context);
        cam = camera;
        mHolder = getHolder();
        mHolder.addCallback(this);
    }

    @Override
    public void surfaceCreated(SurfaceHolder holder) {
        try {
            if (cam != null) {
                cam.setPreviewDisplay(holder);
                cam.startPreview();
            }
        } catch (IOException e) {
            Log.d(VIEW_LOG_TAG, "Error setting camera preview" + e.getMessage());
        }
    }

    @Override
    public void surfaceChanged(SurfaceHolder holder, int format, int w, int h) {
        if (mHolder.getSurface() == null) {
            return;
        }
        try {
            cam.stopPreview();
//            cam.setPreviewDisplay(mHolder);
            cam.startPreview();
        } catch (Exception e) {
            e.printStackTrace();
            Log.d(VIEW_LOG_TAG, "Error starting camera preview ");
        }
    }

    @Override
    public void surfaceDestroyed(SurfaceHolder holder) {
        if (cam == null) return;
        cam.stopPreview();
        cam = null;
    }

    public void resume() {
        if (cam == null) return;
        try {
            cam.setPreviewDisplay(mHolder);
            cam.startPreview();
        } catch (IOException e) {
        }

    }

    public void pause() {
        cam.stopPreview();
        cam = null;
    }
}

