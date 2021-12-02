package com.mp.jaesun_final;

import android.app.Activity;
import android.graphics.Bitmap;
import android.hardware.Camera;
import android.os.Bundle;
import android.os.Handler;
import android.os.Message;
import android.util.Log;
import android.widget.FrameLayout;
import android.widget.TextView;

import androidx.annotation.NonNull;

import java.util.Random;

public class PlayActivity extends Activity {
    public static boolean isOn = false;
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

    Camera.PictureCallback pictureCallback = new Camera.PictureCallback() {
        @Override
        public void onPictureTaken(byte[] data, Camera camera) {
            camera.stopPreview();
            // img processing ...
            Bitmap img = MyBitmap.getImage(data);
            int w = img.getWidth(), h = img.getHeight();
            img = Bitmap.createScaledBitmap(img, w / 4, h / 4, true);
            MyBitmap.rgb2hsv(img); // hsv
            Bitmap redTh = img, greenTh = img.copy(img.getConfig(), true);
            MyBitmap.inRange(redTh, MyBitmap.redRange);
            boolean redUp = MyBitmap.isUp(redTh);
            redTh = img = null;
            boolean greenUp = MyBitmap.isUp(greenTh);
            MyBitmap.inRange(greenTh, MyBitmap.greenRange);
            greenTh = null;

            // UI processing ...
            boolean isCorrect = qMaker.isCorrect(redUp, greenUp);
            Player.p.setIsCorrect(isCorrect);
            tvAnswer.setText("ANSWER: " + qMaker.makeAnsStr());
            tvYourAnswer.setText("YOUR ANSWER: " + qMaker.makeAnsStr(redUp, greenUp));
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

    }

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_play);
        isOn = true;
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

    @Override
    protected void onDestroy() {
        if (Player.p != null)
            Player.p.gameFinish();
        mycam.close();
        super.onDestroy();
        isOn = false;
    }

    class GameThread extends Thread {
        @Override
        public void run() {
            int ret = 1;
            Player.score = 0;
            for (Player.stage = 1; Player.stage <= Player.NUM_STAGE; Player.stage++) {
                soundMana.play(SoundManager.NEXT);
                qMaker.make(Player.level);
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
//            Player.p.gameFinish();

            finishHandler.sendMessage(msg);
        }
    }

}


class Question {
    private final String[] flags = {"청기 ", "홍기 ", "전부 "};
    private final String[] actives1 = {"내리고 ", "올리고 ", "올리지 말고 ", "내리지 말고 "};
    private final String[] actives2 = {"내려", "올려", "올리지 마", "내리지 마"};
    public boolean redUp = false, greenUp = false;
    private Random rand = new Random(System.currentTimeMillis());
    public String quest = "";

    public String make(int level) {
        StringBuilder sb = new StringBuilder(0);
        int ret = 0;
        for (int i = 0; i < level - 1; i++) {
            ret = makeCmd(actives1, ret, sb);
        }
        ret = makeCmd(actives2, ret, sb);
        greenUp = (ret & 1) != 0;
        redUp = (ret & 2) != 0;
        quest = sb.toString();
        return quest;
    }

    private int makeCmd(String[] actives, int ret, StringBuilder sb) {
        int flag = rand.nextInt(3), act = rand.nextInt(4);
        sb.append(flags[flag]).append(actives[act]);
        flag++;
        act %= 2;
        ret = (ret & ~flag) | (act | (act << 1)) & flag;
        return ret;
    }

    public String makeAnsStr() {
        return String.format("red: %s  green: %s", redUp ? "UP" : "DOWN", greenUp ? "UP" : "DOWN");
    }

    public String makeAnsStr(boolean rUp, boolean gUp) {
        return String.format("red: %s  green: %s", rUp ? "UP" : "DOWN", gUp ? "UP" : "DOWN");
    }

    public boolean isCorrect(boolean rUp, boolean gUp) {
        return (redUp == rUp) && (greenUp == gUp);
    }


}

