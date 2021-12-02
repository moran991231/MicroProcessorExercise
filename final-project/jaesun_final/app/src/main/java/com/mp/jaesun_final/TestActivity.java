package com.mp.jaesun_final;

import android.app.Activity;
import android.graphics.Bitmap;
import android.hardware.Camera;
import android.os.Bundle;
import android.util.Log;
import android.widget.Button;
import android.widget.FrameLayout;
import android.widget.ImageView;
import android.widget.TextView;
import android.widget.Toast;

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
        mycam = new MyCamera(this, camPreview,null);

        mycam.pictureCallback = new Camera.PictureCallback() {
            @Override
            public void onPictureTaken(byte[] data, Camera camera) {
                camera.stopPreview();
                Bitmap img = MyBitmap.getImage(data);
                int w = img.getWidth(), h = img.getHeight();
                img = Bitmap.createScaledBitmap(img,w/4, h/4 ,true);
                MyBitmap.rgb2hsv(img); // hsv
                Bitmap redTh = img, greenTh = img.copy(img.getConfig(),true);
                MyBitmap.inRange(redTh, MyBitmap.redRange);
                boolean redUp = MyBitmap.isUp(redTh);
                redTh=img=null;
                MyBitmap.inRange(greenTh, MyBitmap.greenRange);
                boolean greenUp = MyBitmap.isUp(greenTh);
                greenTh=null;

                setFlagText(redUp, greenUp);
                camera.startPreview();
            }
        };

        Button btn =(Button) findViewById(R.id.btnTest);
        btn.setOnClickListener(v->{
            if(!MyBitmap.isCaliAvailable()){
                Toast.makeText(TestActivity.this, "DO CALIB FIRST!",Toast.LENGTH_SHORT).show();
                return;
            }
            mycam.takePicture();
        });

    }

    public void setFlagText(boolean redUp, boolean greenUp){
        tvRed.setText(redUp?"UP":"DOWN");
        tvGreen.setText(greenUp?"UP":"DOWN");
    }

    protected void onDestroy() {
        mycam.close();
        Log.d("MY_CALI_ACT","destroyed~~~~");
        super.onDestroy();
    }
}
