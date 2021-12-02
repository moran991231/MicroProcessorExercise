package com.mp.jaesun_final;

import android.os.Handler;
import android.os.Message;
import android.util.Log;
import android.widget.TextView;

import androidx.appcompat.app.AppCompatActivity;

import java.util.Random;

public class Player {
    public static Player p = null;
    public static int score = 0, level = 1, stage = 0;
    private static final String tag = "PLAYER";
    public static final int NUM_STAGE = 3;
    private PlayActivity.GameThread gameThread = null;
    private boolean isCorrect;

    public void setIsCorrect(boolean isCorrect){
        this.isCorrect = isCorrect;
        if(gameThread==null) return;
        if(gameThread.getState() != Thread.State.RUNNABLE)
            gameThread.interrupt();
        if(isCorrect)score++;
    }
    public boolean getIsCorrect(){return isCorrect;}

    public static String makeScoreStr(){
        return String.format("SCORE: %d",score);
    }

    public static String makeStageStr(){
        return String.format("STAGE: %d / %d", stage, NUM_STAGE);
    }

    public void gameStart(PlayActivity.GameThread gameThread) {
        if (p != null) {
            Log.d(tag, "Game is already started.");
            return;
        }
        p = this;
        p.gameThread = gameThread;
        p.gameThread.start();
    }

    public void gameFinish() {
        p.gameThread.interrupt();
        p.gameThread = null;
        p=null;
    }


    public static String getResultString() {
        if (score < 2) return "POOR";
        else if (score < 4) return "NOT BAD";
        else if (score < 6) return "GOOD";
        else if (score < 8) return "VERY NICE";
        else return "PERFECT";
    }


}