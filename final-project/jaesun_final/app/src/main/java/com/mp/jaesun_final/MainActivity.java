package com.mp.jaesun_final;

import androidx.annotation.NonNull;
import androidx.appcompat.app.AppCompatActivity;

import android.content.Intent;
import android.graphics.Point;
import android.os.Bundle;
import android.os.Handler;
import android.os.Message;
import android.util.Log;
import android.view.Display;
import android.widget.Button;
import android.widget.FrameLayout;
import android.widget.ImageView;
import android.widget.RatingBar;
import android.widget.Toast;

public class MainActivity extends AppCompatActivity {
    RatingBar diffLevel;

    Handler levelHandler = new Handler() {
        @Override
        public void handleMessage(@NonNull Message msg) {
            super.handleMessage(msg);
            int level = (int) diffLevel.getRating() + msg.arg1;
            if (1 <= level && level <= 8) {
                diffLevel.setRating(level);
                Player.level = level;
                BoardIO.led.writeStick(level);
            }
        }
    };

    Handler playHandler = new Handler() {
        @Override
        public void handleMessage(@NonNull Message msg) {
            super.handleMessage(msg);
            if (!MyBitmap.isCaliAvailable()) {
                Toast.makeText(MainActivity.this, "DO CALIB FIRST", Toast.LENGTH_SHORT).show();
                return;
            }
            Toast.makeText(MainActivity.this, "GAME START", Toast.LENGTH_SHORT).show();
            Intent intent = new Intent(getApplicationContext(), PlayActivity.class);
            startActivity(intent);
        }
    };

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);
        diffLevel = (RatingBar) findViewById(R.id.rbLevel);

        makeBoardIOInstances();

        Button btn = (Button) findViewById(R.id.btnCaliShow);
        btn.setOnClickListener(v -> {
            Intent intent = new Intent(getApplicationContext(), CaliActivity.class);
            startActivity(intent);
        });
        btn = (Button) findViewById(R.id.btnTestShow);
        btn.setOnClickListener(v -> {
            Intent intent = new Intent(getApplicationContext(), TestActivity.class);
            startActivity(intent);
        });

        btn = (Button) findViewById(R.id.btnDebug);
        btn.setOnClickListener(v -> {
            Intent intent = new Intent(getApplicationContext(), PlayActivity.class);
            startActivity(intent);

        });

    }

    private void makeBoardIOInstances() {
        if (BoardIO.gpioBtn == null)
            BoardIO.gpioBtn = new GpioButton(new DoGpioButtonClicked() {
                @Override
                public void doThis(int directoin) {
                    Message msg = Message.obtain();
                    if (directoin == GpioButton.DOWN) {
                        msg.arg1 = -1;
                        levelHandler.sendMessage(msg);
                    } else if (directoin == GpioButton.UP) {
                        msg.arg1 = 1;
                        levelHandler.sendMessage(msg);
                    } else if (directoin == GpioButton.CENTER) {
                        playHandler.sendMessage(msg);
                    }
                }
            });
        if (BoardIO.led == null)
            BoardIO.led = new Led();
        if (BoardIO.sevSeg == null)
            BoardIO.sevSeg = new SevenSegment();
    }


    @Override
    protected void onResume() {
        super.onResume();
        makeBoardIOInstances();
        BoardIO.gpioBtn.open();
        BoardIO.led.open();
        BoardIO.led.writeStick(Player.level);
        BoardIO.sevSeg.open();
    }

    @Override
    protected void onPause() {
//        BoardIO.gpioBtn.close();
//        BoardIO.led.close();
//        BoardIO.sevSeg.close();
        super.onPause();

    }

    @Override
    protected void onDestroy() {
        BoardIO.led.writeStick(0);
        BoardIO.gpioBtn.close();
        BoardIO.led.close();
        BoardIO.sevSeg.close();
        super.onDestroy();
    }
}