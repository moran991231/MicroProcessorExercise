package com.example.gaussian_blur_jni;

import androidx.appcompat.app.AppCompatActivity;

import android.graphics.Bitmap;
import android.graphics.BitmapFactory;
import android.os.Bundle;
import android.util.Log;
import android.widget.Button;
import android.widget.ImageView;
import android.widget.TextView;

import com.example.gaussian_blur_jni.databinding.ActivityMainBinding;

public class MainActivity extends AppCompatActivity {

    // Used to load the 'gaussian_blur_jni' library on application startup.
    static {
        System.loadLibrary("OpenCLDriver");
    }
    public native Bitmap GaussianBlurBitmap(Bitmap bitmap);
    public native Bitmap GaussianBlurBitmapGpu(Bitmap bitmap);


    private ActivityMainBinding binding;
    ImageView imgV;
    Bitmap buf_bitmap;
    Bitmap orig_bitmap;
    TextView tvTime;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);
        tvTime = (TextView) findViewById(R.id.tvTime);
        imgV = (ImageView) findViewById(R.id.imageView);
        Button bCpu = (Button) findViewById(R.id.bCpu);
        Button bGpu = (Button) findViewById(R.id.bGpu);
        Button bOrig = (Button) findViewById(R.id.bOrig);

        BitmapFactory.Options options = new BitmapFactory.Options();
        options.inPreferredConfig = Bitmap.Config.ARGB_8888;

        buf_bitmap = BitmapFactory.decodeFile("/data/local/tmp/lena.bmp",options);
        orig_bitmap = buf_bitmap.copy(options.inPreferredConfig, true);
        Log.d("MY_LOG", String.format("@@@ w: %d, h: %d ",buf_bitmap.getWidth(),buf_bitmap.getHeight()));
        imgV.setImageBitmap(buf_bitmap);

        bCpu.setOnClickListener(v->{
            float start = (float) System.nanoTime()/1_000_000L;
//            buf_bitmap = buf_bitmap.copy(options.inPreferredConfig, true);
            GaussianBlurBitmap(buf_bitmap);
            imgV.setImageBitmap(buf_bitmap);
            float end = (float) System.nanoTime()/1_000_000L;
            float timesub = end-start;
            tvTime.setText("Execution time: "+timesub+" ms");

        });
        bGpu.setOnClickListener(v->{
            float start = (float) System.nanoTime()/1_000_000L;

            GaussianBlurBitmapGpu(buf_bitmap);

            imgV.setImageBitmap(buf_bitmap);
            float end = (float) System.nanoTime()/1_000_000L;
            float timesub = end-start;
            tvTime.setText("Execution time: "+timesub+" ms");

        });
        bOrig.setOnClickListener(v->{
            tvTime.setText("Execution time: ");
            buf_bitmap = orig_bitmap.copy(options.inPreferredConfig, true);
            imgV.setImageBitmap(orig_bitmap);
        });


    }

}