package com.mp.jaesun_final;

public class Player {
    public static Player p = null;
    public static int score = 0, level = 1, stage = 0, realLevel = 1;
    private static final String tag = "PLAYER";
    public static final int NUM_STAGE = 5;
    private PlayActivity.GameThread gameThread = null;
    private boolean isCorrect;

    public void setIsCorrect(boolean isCorrect) {
        this.isCorrect = isCorrect;
        if (gameThread == null) return;
        if (gameThread.getState() != Thread.State.RUNNABLE)
            gameThread.interrupt();
        if (isCorrect) score += realLevel;
    }

    public boolean getIsCorrect() {
        return isCorrect;
    }

    public static String makeScoreStr() {
        return String.format("SCORE: %d", score);
    }

    public static String makeStageStr() {
        return String.format("STAGE: %d / %d", stage, NUM_STAGE);
    }

    public void gameStart(PlayActivity.GameThread gameThread) {
        if (p != null) {
            return;
        }
        p = this;
        p.gameThread = gameThread;
        p.gameThread.start();
    }

    public void gameFinish() {
        p.gameThread.interrupt();
        p.gameThread = null;
        p = null;
    }


    public static String getResultString() {
        if (score == NUM_STAGE) return "PERFECT";
        else if (score >= 4 * NUM_STAGE / NUM_STAGE) return "VERY NICE";
        else if (score >= 3 * NUM_STAGE / NUM_STAGE) return "GOOD";
        else if (score >= 2 * NUM_STAGE / NUM_STAGE) return "NOT BAD";
        else return "POOR";
    }


}