package com.mp.jaesun_final.helper;

import java.util.Random;

public
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

