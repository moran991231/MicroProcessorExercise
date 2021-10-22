package com.example.threadexample;

import androidx.appcompat.app.AppCompatActivity;

import android.os.Bundle;

public class MainActivity extends AppCompatActivity {

    class SimpleThread extends Thread{
        public void run(){
            while(true){
                System.out.println(System.currentTimeMillis()+" " +getName());
                try {
                    sleep(1000);
                } catch (InterruptedException e) {
                    e.printStackTrace();
                }
            }
        }
    }
    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        SimpleThread st1 = new SimpleThread();
        SimpleThread st2 = new SimpleThread();
        st1.start();
        st2.start();
    }
}