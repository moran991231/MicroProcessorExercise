package com.mp.jaesun_final;

import android.content.Context;
import android.graphics.Bitmap;
import android.graphics.BitmapFactory;
import android.graphics.Matrix;
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
    private ImageView capturedView;

    public MyCamera(MainActivity main) {
        mtx_180.postRotate(180);
        imgView = main.camPreview;
        capturedView = main.capturedView;
    }

    public void open(MainActivity main) {
        if (camera != null) return;
        camera = getCameraInstance();
        camera.setDisplayOrientation(180);
        preview = new CameraPreview(main, camera);
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

    private final Matrix mtx_180 = new Matrix();
    private final int W = 800, H = 480;
    Camera.PictureCallback pictureCallback = new Camera.PictureCallback() {
        @Override
        public void onPictureTaken(byte[] data, Camera camera) {
            camera.stopPreview();
            Bitmap img = BitmapFactory.decodeByteArray(data, 0, data.length);
            Log.d("CAMERA", String.format("orig img sisze: %d x %d", img.getWidth(), img.getHeight()));
            img = Bitmap.createScaledBitmap(img, W, H, true);
            img = Bitmap.createBitmap(img, 0, 0, W, H, mtx_180, true);
            capturedView.setImageBitmap(img);
            camera.startPreview();
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
        if (mHolder.getSurface() == null) {
            return;
        }
        try {
            cam.stopPreview();
//            cam.setPreviewDisplay(mHolder);

            Camera.Parameters params = cam.getParameters();
//            List<Camera.Size> list = params.getSupportedPictureSizes();
//            for (Camera.Size size : list) {
//                Log.d("CAMERA", String.format("picture sizes; %d x %d", size.width, size.height));
//            }
            params.setPictureSize(800, 480);
            params.setPreviewSize(800, 480);
            cam.setParameters(params);
            cam.startPreview();
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
        cam.stopPreview();
        cam = null;
    }
}

