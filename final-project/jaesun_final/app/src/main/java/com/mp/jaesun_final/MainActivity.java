package com.mp.jaesun_final;

import androidx.annotation.NonNull;
import androidx.appcompat.app.AppCompatActivity;

import android.content.Intent;
import android.os.Bundle;
import android.os.Handler;
import android.os.Message;
import android.widget.Button;
import android.widget.RatingBar;
import android.widget.Toast;

import com.mp.jaesun_final.helper.MyBitmap;
import com.mp.jaesun_final.ioDevices.BoardIO;
import com.mp.jaesun_final.ioDevices.DoGpioButtonClicked;
import com.mp.jaesun_final.ioDevices.GpioButton;

public class MainActivity extends AppCompatActivity {
    RatingBar diffLevel;
    boolean gameAvailable = false;

    private void showToast(String str) {
        Toast.makeText(this, str, Toast.LENGTH_SHORT).show();
    }

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
                showToast("DO CALIB FIRST");
                return;
            }
            if (!gameAvailable) return;
            showToast("GAME START");
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

        gameAvailable = true;

    }

    private void makeBoardIOInstances() {
        if (BoardIO.gpioBtn == null) {
            BoardIO.gpioBtn = new GpioButton("/dev/sm9s5422_interrupt", BoardIO.O_RDONLY);
        }

        BoardIO.gpioBtn.setAfterClick(new DoGpioButtonClicked() {
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

        diffLevel.setRating(Player.level);
    }


    @Override
    protected void onResume() {
        gameAvailable = true;
        super.onResume();
        makeBoardIOInstances();
        BoardIO.gpioBtn.open();
        BoardIO.led.open();
        BoardIO.led.writeStick(Player.level);
        BoardIO.sevSeg.open();
    }

    @Override
    protected void onPause() {
        gameAvailable = false;
//        BoardIO.gpioBtn.close();
//        BoardIO.led.close();
//        BoardIO.sevSeg.close();
        super.onPause();

    }

    @Override
    protected void onDestroy() {
        gameAvailable = false;
        BoardIO.gpioBtn.close();
        BoardIO.led.close();
        BoardIO.sevSeg.close();
        super.onDestroy();
    }
}