package com.mp.jaesun_final;

import androidx.appcompat.app.AppCompatActivity;

import android.os.Bundle;

public class MainActivity extends AppCompatActivity {
    GpioButton gpioBtn;
    Led led;
    SevenSegment sevenseg;
    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

         gpioBtn = new GpioButton();
         led = new Led();
         sevenseg = new SevenSegment();


    }



}