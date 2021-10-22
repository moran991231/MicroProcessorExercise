package com.example.new_led_test;

import androidx.appcompat.app.AppCompatActivity;

import android.os.Bundle;
import android.widget.*;



public class MainActivity extends AppCompatActivity {

    // Used to load the 'led_example' library on application startup.
    static {
        System.loadLibrary("JNIDriver");
    }

    private native static int openDriver(String path);
    private native static void closeDriver();
    private native static void writeDriver(byte[] data, int length);

    int[] btString=new int[8];
    ToggleButton[] mBtn = new ToggleButton[8];
    byte data[] = {0,0,0,0,0,0,0,0};

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        CompoundButton.OnCheckedChangeListener listener = new CompoundButton.OnCheckedChangeListener() {
            @Override
            public void onCheckedChanged(CompoundButton buttonView, boolean isChecked) {
                for(int i=0; i<8; i++){
                    if(btString[i] == buttonView.getId()){
                        data[i] =(byte)( isChecked?1:0);
                        break;
                    }
                }
                writeDriver(data,data.length);
            }
        };
        btString = new int[]{R.id.toggleButton0,R.id.toggleButton1,R.id.toggleButton2,R.id.toggleButton3,
                R.id.toggleButton4,R.id.toggleButton5,R.id.toggleButton6,R.id.toggleButton7};
        for(int i=0; i<8; i++){
            mBtn[i] = (ToggleButton) findViewById(btString[i]);
            mBtn[i].setChecked(false);
            mBtn[i].setOnCheckedChangeListener(listener);
        }
    }

    protected void onPause(){
        closeDriver();
        super.onPause();
    }
    protected  void onResume(){
        if(openDriver("/dev/sm9s5422_led")<0){
            Toast.makeText(MainActivity.this,"Driver Open Failed", Toast.LENGTH_SHORT).show();
        }
        super.onResume();
    }
}