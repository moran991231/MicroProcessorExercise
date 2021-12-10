package com.mp.jaesun_final.helper;

import java.util.Collections;
import java.util.LinkedList;
import java.util.StringTokenizer;

public class Ranking {
    public static final String FILE_NAME = "ranking.txt";
    LinkedList<Record> list = new LinkedList<Record>();

    public void Ranking(String text) {
        String[] lines = text.split("\n");
        for (String line : lines)
            if (line.length() > 1)
                list.add(new Record(line));
    }

    public void addRecord(String line) {
        Record temp = new Record(line);
        for (Record r : list) {
            if (r.equalsTo(temp)) return; // already record exist
        }
        list.add(temp);
    }

    public void addRecord(String name, int score) {
        Record temp = new Record(name, score);
        for (Record r : list) {
            if (r.equalsTo(temp)) return; // already record exist
        }
        list.add(temp);
        Collections.sort(list);
    }

    public String toString(boolean isNum) {
        Collections.sort(list);
        StringBuilder sb = new StringBuilder();
        int len = Math.min(10, list.size());
        for (int i = 0; i < len; i++) {
            if (isNum) {
                sb.append(i + 1).append(' ');
            }
            sb.append(list.get(i).toString()).append('\n');
        }
        return sb.toString();
    }

}

class Record implements Comparable<Record> {
    String name;
    int score;

    public Record(String line) {
        StringTokenizer st = new StringTokenizer(line);
        name = st.nextToken();
        score = Integer.parseInt(st.nextToken());
    }

    public Record(String n, int s) {
        name = n;
        score = s;
    }

    public String toString() {
        return String.format("%10s %2d", name, score);
    }

    public boolean equalsTo(Record o) {
        return score == o.score && name.compareTo(o.name) == 0;
    }

    @Override
    public int compareTo(Record o) {
        return -score + o.score;
    }
}
