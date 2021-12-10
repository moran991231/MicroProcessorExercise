package com.mp.jaesun_final;

import android.app.Activity;
import android.hardware.Camera;
import android.os.Bundle;
import android.widget.Button;
import android.widget.FrameLayout;
import android.widget.TextView;
import android.widget.Toast;

import com.mp.jaesun_final.helper.FlagState;
import com.mp.jaesun_final.helper.MyBitmap;

public class TestActivity extends Activity {
    FrameLayout camPreview;
    MyCamera mycam;
    TextView tvRed, tvGreen;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_test);

        tvRed = (TextView) findViewById(R.id.tvRedTest);
        tvGreen = (TextView) findViewById(R.id.tvGreenTest);

        camPreview = (FrameLayout) findViewById(R.id.camPreviewTest);
        mycam = new MyCamera(this, camPreview, null);

        mycam.pictureCallback = new Camera.PictureCallback() {
            @Override
            public void onPictureTaken(byte[] data, Camera camera) {
                camera.stopPreview();

                FlagState fs = MyBitmap.getResult(data);
                setFlagText(fs.redUp, fs.greenUp);
                camera.startPreview();
            }
        };

        Button btn = (Button) findViewById(R.id.btnTest);
        btn.setOnClickListener(v -> {
            if (!MyBitmap.isCaliAvailable()) {
                Toast.makeText(TestActivity.this, "DO CALIB FIRST!", Toast.LENGTH_SHORT).show();
                return;
            }
            mycam.takePicture();
        });

    }

    public void setFlagText(boolean redUp, boolean greenUp) {
        tvRed.setText(redUp ? "UP" : "DOWN");
        tvGreen.setText(greenUp ? "UP" : "DOWN");
    }

    protected void onDestroy() {
        mycam.close();
        super.onDestroy();
    }
}
