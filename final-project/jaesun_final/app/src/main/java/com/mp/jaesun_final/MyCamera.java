package com.mp.jaesun_final;

import android.content.Context;
import android.hardware.Camera;
import android.util.Log;
import android.view.SurfaceHolder;
import android.view.SurfaceView;
import android.widget.FrameLayout;
import android.widget.ImageView;

import java.io.IOException;

public class MyCamera {

    private Camera camera;
    private CameraPreview preview;
    private FrameLayout imgView;
    private ImageView capturedView;

    public MyCamera(Context context, FrameLayout imgView, ImageView capturedView) {
        this.imgView = imgView;
        this.capturedView = capturedView;
        open(context);
    }

    public void open(Context context) {
        if (camera != null) return;
        camera = getCameraInstance();
        camera.setDisplayOrientation(180);
        preview = new CameraPreview(context, camera);
        imgView.addView(preview, 0);

    }

    public void close() {
        if (camera == null) return;
        imgView.removeViewAt(0);
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

    public int mode = 0;
    public Camera.PictureCallback pictureCallback = new Camera.PictureCallback() {
        @Override
        public void onPictureTaken(byte[] data, Camera camera) {
        }
    };

    public void takePicture() {
        camera.takePicture(null, null, pictureCallback);
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
    public void surfaceChanged(SurfaceHolder holder, int format, int w, int h) {
        if (cam == null) return;
        if (mHolder.getSurface() == null) {
            return;
        }
        try {
            Camera.Parameters params = cam.getParameters();
            params.setPictureSize(800, 600);
            params.setPreviewSize(800, 600);
            cam.setParameters(params);
        } catch (Exception e) {
            e.printStackTrace();
            Log.d(VIEW_LOG_TAG, "Error starting camera preview ");
        }
    }

    @Override
    public void surfaceCreated(SurfaceHolder holder) {
        if (cam != null) {
            resume();
        }
    }

    @Override
    public void surfaceDestroyed(SurfaceHolder holder) {
        if (cam == null) return;
        pause();
    }

    public void resume() {
        try {
            cam.setPreviewDisplay(mHolder);
            cam.startPreview();
        } catch (IOException e) {
            e.printStackTrace();
        }
    }

    public void pause() {
        if (cam == null) return;
        cam.stopPreview();
        cam = null;
    }
}

