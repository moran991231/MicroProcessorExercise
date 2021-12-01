package com.mp.jaesun_final;

import androidx.annotation.NonNull;
import androidx.appcompat.app.AppCompatActivity;

import android.graphics.Point;
import android.media.Image;
import android.os.Bundle;
import android.os.Handler;
import android.os.Message;
import android.util.Log;
import android.view.Display;
import android.widget.Button;
import android.widget.FrameLayout;
import android.widget.ImageView;
import android.widget.RatingBar;

public class MainActivity extends AppCompatActivity {
    GpioButton gpioBtn;
    Led led;
    SevenSegment sevenseg;
    FrameLayout camPreview;
    MyCamera mycam;
    ImageView capturedView;
    RatingBar diffLevel;
    int a = 0, b = 0;

    Handler levelHandler = new Handler(){
        @Override
        public void handleMessage(@NonNull Message msg){
            super.handleMessage(msg);
            int level = (int)diffLevel.getRating()+msg.arg1;
            if(1<=level && level<=8)
                diffLevel.setRating(level);
        }
    };

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);
        capturedView = (ImageView)findViewById(R.id.capturedView);
        diffLevel = (RatingBar)findViewById(R.id.rbLevel);
        Display display = getWindowManager().getDefaultDisplay();
        Point size = new Point();
        display.getSize(size);
        Log.d("MAIN", String.format("win size: %dx%d", size.x, size.y));
        gpioBtn = new GpioButton(new DoGpioButtonClicked() {
            @Override
            public void doThis(int directoin) {
                Message msg = Message.obtain();
                if(directoin == GpioButton.DOWN)
                    msg.arg1=-1;
                else if (directoin== GpioButton.UP)
                    msg.arg1=1;
                levelHandler.sendMessage(msg);

            }
        });
        led = new Led();
        sevenseg = new SevenSegment();

        camPreview = (FrameLayout) findViewById(R.id.camPreview);
        mycam = new MyCamera(this);
        mycam.open(this);

        Button btn = (Button) findViewById(R.id.btnSeg);
        btn.setOnClickListener(v -> {
            if(a==0)
                sevenseg.write(SevenSegment.GOOD__,20);
            else if (a==1)
                sevenseg.write(SevenSegment.BAD___,20);
            else
                sevenseg.write(new byte[]{0, 1, 2, 3, 4, 5},20);
            a = (a+1)%3;
        });

        btn = (Button) findViewById(R.id.btnLed);
        btn.setOnClickListener(v -> {
            led.writeStick(b);
            b++;
            if (b > 8) b = 0;
        });

        btn = (Button) findViewById(R.id.btnCapture);
        btn.setOnClickListener(v->{
            mycam.mode=0;
            mycam.takePicture();
        });

        btn = (Button) findViewById(R.id.btnThresh);
        btn.setOnClickListener(v->{
            mycam.mode=1;
            mycam.takePicture();

        });


    }

    @Override
    protected void onPause() {
        gpioBtn.close();
        led.close();
        sevenseg.close();
        mycam.close();
        super.onPause();
    }

    @Override
    protected void onResume() {
        super.onResume();
        mycam.open(this);
        gpioBtn.open();
        led.open();
        sevenseg.open();
    }
}