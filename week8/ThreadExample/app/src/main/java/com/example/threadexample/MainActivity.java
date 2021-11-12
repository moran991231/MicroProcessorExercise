package com.example.threadexample;

import androidx.appcompat.app.AppCompatActivity;
import android.os.Bundle;
import java.text.SimpleDateFormat;
import java.util.Date;

public class MainActivity extends AppCompatActivity {

    class SimpleThread extends Thread{
        private SimpleDateFormat sdf = new SimpleDateFormat("yyyy-MM-dd-HH:mm:ss");
        String time;
        public void run(){
            while(true){
                time = sdf.format(new Date(System.currentTimeMillis()));
                System.out.println(time+" " +getName());
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