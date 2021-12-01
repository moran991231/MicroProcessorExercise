package com.mp.jaesun_final;

import androidx.appcompat.app.AppCompatActivity;

import android.graphics.Point;
import android.media.Image;
import android.os.Bundle;
import android.util.Log;
import android.view.Display;
import android.widget.Button;
import android.widget.FrameLayout;
import android.widget.ImageView;

public class MainActivity extends AppCompatActivity {
    GpioButton gpioBtn;
    Led led;
    SevenSegment sevenseg;
    FrameLayout camPreview;
    MyCamera mycam;
    ImageView capturedView;
    int a = 0, b = 0;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);
        capturedView = (ImageView)findViewById(R.id.capturedView);
        Display display = getWindowManager().getDefaultDisplay();
        Point size = new Point();
        display.getSize(size);
        Log.d("MAIN", String.format("win size: %dx%d", size.x, size.y));
        gpioBtn = new GpioButton();
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
        super.onPause();
    }

    @Override
    protected void onResume() {
        super.onResume();
        gpioBtn.open();
        led.open();
        sevenseg.open();
    }
}