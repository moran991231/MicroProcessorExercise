package com.example.camera_api;

import androidx.appcompat.app.AppCompatActivity;

import android.os.Bundle;
import android.graphics.*;
import android.hardware.Camera;
import android.view.*;
import android.widget.*;
import android.hardware.Camera.PictureCallback;

public class MainActivity extends AppCompatActivity {

    private Camera mCamera;
    private CameraPreview mPreview;
    private ImageView capturedImageHolder;
    private Matrix mtx_180 = new Matrix();
    @Override
    protected void onCreate(Bundle savedInstanceState) {
        // set GUI components, and get them
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);
        Button btn = (Button) findViewById(R.id.button_cpature);
        capturedImageHolder = (ImageView) findViewById(R.id.capture_img);
        FrameLayout preview = (FrameLayout) findViewById(R.id.camera_preview);

        // settings...
        mtx_180.postRotate(180); // rotation matrix
        initCamera();
        mPreview = new CameraPreview(this, mCamera);
        preview.addView(mPreview);
        btn.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                mCamera.takePicture(null, null, pictureCallback);
            }
        });
    }

    public static Camera getCameraInstance() {
        try {
            return Camera.open();
        } catch (Exception e) {
            return null;
        }
    }
    private final int W=450, H=300;
    PictureCallback pictureCallback = new PictureCallback() {
        @Override
        public void onPictureTaken(byte[] data, Camera camera) {
            Bitmap img = BitmapFactory.decodeByteArray(data, 0, data.length); // 1. get img
            img = Bitmap.createScaledBitmap(img, W, H,true);// 2. scale down
            img = Bitmap.createBitmap(img, 0, 0, W, H, mtx_180, true); //3. rotate 180
            capturedImageHolder.setImageBitmap(img);//4. set captured img
        }
    };

    @Override
    protected void onPause() {
        super.onPause();
        releaseCamera();
    }
    private void releaseCamera() {
        if (mCamera != null) {
            mPreview.pause();
            mCamera.release();
            mCamera = null;
        }
    }

    @Override
    protected void onResume() {
        super.onResume();
        if (mCamera == null) {
            initCamera();
            mPreview.setCamera(mCamera);
            mPreview.resume();
        }
    }
    public void initCamera() {
        if (mCamera != null) return;
        mCamera = getCameraInstance();
        mCamera.setDisplayOrientation(180);
    }
}