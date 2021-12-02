package com.mp.jaesun_final;

import android.app.Activity;
import android.graphics.Bitmap;
import android.hardware.Camera;
import android.os.Bundle;
import android.util.Log;
import android.widget.Button;
import android.widget.FrameLayout;
import android.widget.ImageView;
import android.widget.RadioGroup;
import android.widget.Toast;

public class CaliActivity extends Activity {

    FrameLayout camPreview;
    ImageView capturedView;
    MyCamera mycam;
    boolean isCaliForRed=true;
    final int SET_RANGE=0, TEST_THESHOLD=1;
    int mode=SET_RANGE;
    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_cali);

        camPreview = (FrameLayout) findViewById(R.id.camPreviewCali);
        capturedView = (ImageView)findViewById(R.id.ivCali);
        mycam = new MyCamera(this, camPreview,capturedView);

        Button btn = (Button) findViewById(R.id.btnCali);
        btn.setOnClickListener(v->{
            mode = SET_RANGE;
            mycam.takePicture();
        });
        btn = (Button) findViewById(R.id.btnThreshCali);
        btn.setOnClickListener(v->{
            mode = TEST_THESHOLD;
            mycam.takePicture();
        });
        RadioGroup rg = (RadioGroup)   findViewById(R.id.rgCali);
        rg.setOnCheckedChangeListener(new RadioGroup.OnCheckedChangeListener(){
            @Override
            public void onCheckedChanged(RadioGroup group, int checkedId) {
                if(checkedId == R.id.rbRed)
                    isCaliForRed=true;
                else
                    isCaliForRed=false;
                capturedView.setImageResource(0);
            }
        });
        mycam.pictureCallback = new Camera.PictureCallback() {
            @Override
            public void onPictureTaken(byte[] data, Camera camera) {
                camera.stopPreview();
                Bitmap img = MyBitmap.getImage(data);
                int temp = mode;
                byte[] range;
                switch(mode){
                    case SET_RANGE:
                        img = MyBitmap.getCrop(img);
                        Bitmap tempImg = img.copy(img.getConfig(),true);
                         range = MyBitmap.getHsvRange(tempImg);
                        if(isCaliForRed)
                            MyBitmap.redRange = range;
                        else
                            MyBitmap.greenRange = range;
                        break;
                    case TEST_THESHOLD:
                        range = isCaliForRed?MyBitmap.redRange:MyBitmap.greenRange;
                        if(range==null){
                            Log.d("MY_CALI_ACT", "RANGE IS NULL");
                            Toast.makeText(CaliActivity.this,"DO CALI BEFORE TEST",Toast.LENGTH_SHORT).show();
                            break;
                        }
                        MyBitmap.rgb2hsv(img);
                        MyBitmap.inRange(img, range);
                        break;
                }
                if(MyBitmap.isCaliAvailable())
                    Toast.makeText(CaliActivity.this,"CALI COMPLETED! NOW GO TO PLAY!",Toast.LENGTH_SHORT).show();

                capturedView.setImageBitmap(img);
                camera.startPreview();

            }
        };

    }

    @Override
    protected void onDestroy() {
        mycam.close();
        Log.d("MY_CALI_ACT","destroyed~~~~");
        super.onDestroy();
    }
}
