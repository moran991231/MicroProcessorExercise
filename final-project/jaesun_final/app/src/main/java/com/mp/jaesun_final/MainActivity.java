package com.mp.jaesun_final;

import androidx.appcompat.app.AppCompatActivity;

import android.os.Bundle;
import android.widget.Button;

public class MainActivity extends AppCompatActivity {
    GpioButton gpioBtn;
    Led led;
    SevenSegment sevenseg;
    int a=0, b=0;
    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

         gpioBtn = new GpioButton();
         led = new Led();
         sevenseg = new SevenSegment();

         Button btn = (Button) findViewById(R.id.btnSeg);
         btn.setOnClickListener(v->{
             if(a%3==0)
                 sevenseg.updateBuffer(SevenSegment.GOOD__);
             else if(a%3==1)
                 sevenseg.updateBuffer(SevenSegment.BAD___);
             else if(a%3==2)
                 sevenseg.updateBuffer(new byte[]{0,1,2,3,4,5});
             a++;
         });

         btn = (Button)    findViewById(R.id.btnLed);
         btn.setOnClickListener(v->{
             led.writeStick(b);
             b++;
             if(b>8)b=0;
         });



    }

    @Override
    protected void onPause() {
        gpioBtn.close();
        led.close();
        sevenseg.close();
        super.onPause();
    }
}