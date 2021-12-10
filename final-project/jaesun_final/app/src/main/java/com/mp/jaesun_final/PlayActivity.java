package com.mp.jaesun_final;

import android.app.Activity;
import android.graphics.Bitmap;
import android.hardware.Camera;
import android.os.Bundle;
import android.os.Handler;
import android.os.Message;
import android.util.Log;
import android.view.View;
import android.widget.Button;
import android.widget.EditText;
import android.widget.FrameLayout;
import android.widget.TextView;
import android.widget.Toast;

import androidx.annotation.NonNull;

import com.mp.jaesun_final.helper.FlagState;
import com.mp.jaesun_final.helper.MyBitmap;
import com.mp.jaesun_final.helper.Question;
import com.mp.jaesun_final.helper.Ranking;
import com.mp.jaesun_final.ioDevices.BoardIO;
import com.mp.jaesun_final.ioDevices.SevenSegment;
import com.mp.jaesun_final.ioDevices.SoundManager;

import java.io.BufferedReader;
import java.io.IOException;
import java.io.InputStreamReader;
import java.io.OutputStreamWriter;
import java.util.Random;

public class PlayActivity extends Activity {
    TextView tvQuest, tvAnswer, tvYourAnswer, tvScore, tvStage;
    FrameLayout camPreviewTest;

    MyCamera mycam;
    Question qMaker = new Question();
    SoundManager soundMana;

    Handler stageHandler = new Handler() {
        @Override
        public void handleMessage(@NonNull Message msg) {
            super.handleMessage(msg);
            tvStage.setText(Player.makeStageStr());
            tvQuest.setText(qMaker.quest);
            tvAnswer.setText("-");
            tvYourAnswer.setText("-");

        }
    };

    Handler finishHandler = new Handler() {
        @Override
        public void handleMessage(@NonNull Message msg) {
            super.handleMessage(msg);
            if (msg.arg1 > 0)
                showResult();
            else
                PlayActivity.this.finish();
        }
    };


    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_play);
        getUiInstances();
        mycam = new MyCamera(this, camPreviewTest, null);
        mycam.pictureCallback = pictureCallback;

        try {
            Thread.sleep(1000);
        } catch (InterruptedException e) {
        }
        new Player().gameStart(new GameThread());

    }

    private void getUiInstances() {
        tvQuest = (TextView) findViewById(R.id.tvQuest);
        tvAnswer = (TextView) findViewById(R.id.tvAnswer);
        tvYourAnswer = (TextView) findViewById(R.id.tvYourAnswer);
        tvScore = (TextView) findViewById(R.id.tvScore);
        tvStage = (TextView) findViewById(R.id.tvStage);

        camPreviewTest = (FrameLayout) findViewById(R.id.camPreviewTest);

        soundMana = new SoundManager(this);
    }

    Camera.PictureCallback pictureCallback = new Camera.PictureCallback() {
        @Override
        public void onPictureTaken(byte[] data, Camera camera) {
            camera.stopPreview();
            // img processing ...
            FlagState fs = MyBitmap.getResult(data);

            // UI processing ...
            boolean isCorrect = qMaker.isCorrect(fs.redUp, fs.greenUp);
            Player.p.setIsCorrect(isCorrect);
            tvAnswer.setText("ANSWER: " + qMaker.makeAnsStr());
            tvYourAnswer.setText("YOU   : " + qMaker.makeAnsStr(fs.redUp, fs.greenUp));
            tvScore.setText(Player.makeScoreStr());

            camera.startPreview();

        }
    };

    private void showResult() {
        soundMana.play(SoundManager.FINISH);
        setContentView(R.layout.activity_result);
        TextView tv = (TextView) findViewById(R.id.tvScoreResult);
        tv.setText(Player.score + "");
        tv = (TextView) findViewById(R.id.tvResultText);
        tv.setText(Player.getResultString());


        EditText et = (EditText) findViewById(R.id.etRanking);
        Ranking rnk = new Ranking();
        try {
            InputStreamReader fis = new InputStreamReader(openFileInput(Ranking.FILE_NAME));
            BufferedReader br = new BufferedReader(fis);
            while (br.ready())
                rnk.addRecord(br.readLine());
            et.setText(rnk.toStringShow());
            fis.close();

        } catch (IOException e) {
        }

        Button btn = (Button) findViewById(R.id.btnName);
        btn.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                Button b = (Button) v;
                EditText etName = (EditText) findViewById(R.id.etName);
                String name = etName.getText().toString();
                if(name.length()==0){
                    Toast.makeText(PlayActivity.this,"Name is too short",Toast.LENGTH_SHORT).show();
                    return;
                }else if(name.charAt(0)==' '){
                    Toast.makeText(PlayActivity.this,"Don't include space in the name",Toast.LENGTH_SHORT).show();
                    return;
                }
                rnk.addRecord(name, Player.score);
                try {
                    OutputStreamWriter fos = new OutputStreamWriter(PlayActivity.this.openFileOutput(Ranking.FILE_NAME, MODE_PRIVATE));
                    fos.write(rnk.toString());
                    fos.flush();
                    fos.close();
                } catch (IOException e) {
                    e.printStackTrace();
                }

                et.setText(rnk.toStringShow());
                b.setEnabled(false);
            }
        });

    }

    @Override
    protected void onDestroy() {
        if (Player.p != null)
            Player.p.gameFinish();
        mycam.close();
        super.onDestroy();
    }

    class GameThread extends Thread {
        @Override
        public void run() {
            try {
                int ret = 1;
                Player.score = 0;
                Player.numCorrect=0;
                for (Player.stage = 1; Player.stage <= Player.NUM_STAGE; Player.stage++) {
                    soundMana.play(SoundManager.NEXT);
                    Player.realLevel = Player.level;
                    qMaker.make(Player.realLevel);
                    stageHandler.sendMessage(Message.obtain());
                    try {
                        sleep(3000 + 500 * Player.level);
                    } catch (InterruptedException e) {
                        ret = -1;
                        break;
                    }
                    mycam.takePicture();
                    try {
                        sleep(1 * 60 * 1000); // 1min
                    } catch (InterruptedException e) {
                    }
                    boolean isCorrect = Player.p.getIsCorrect();
                    soundMana.play(isCorrect ? SoundManager.GOOD : SoundManager.BAD);
                    BoardIO.sevSeg.write(isCorrect ? SevenSegment.GOOD__ : SevenSegment.BAD___, 80);

                    try {
                        sleep(1000l);
                    } catch (InterruptedException e) {
                        ret = -1;
                        break;
                    }
                }
                Log.d("MY_PLAY_ACT", "game thread finished");
                Message msg = Message.obtain();
                msg.arg1 = ret;

                finishHandler.sendMessage(msg);
            } catch (Exception e) {
                return;
            }
        }
    }

}

