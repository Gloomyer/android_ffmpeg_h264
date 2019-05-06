package com.gloomyer.h264;

import android.Manifest;
import android.graphics.Bitmap;
import android.graphics.Canvas;
import android.graphics.Color;
import android.graphics.PixelFormat;
import android.graphics.Rect;
import android.os.Environment;
import android.support.v4.app.ActivityCompat;
import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;
import android.util.Log;
import android.view.SurfaceView;
import android.view.View;
import android.widget.TextView;

import com.gloomyer.h264.jni.JNIBridge;

import java.io.File;
import java.io.FileInputStream;
import java.io.FileNotFoundException;
import java.io.FileOutputStream;
import java.nio.ByteBuffer;

public class MainActivity extends AppCompatActivity {


    private static final String TAG = "MainActivity";
    private SurfaceView surface;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);
        ActivityCompat.requestPermissions(this,
                new String[]{Manifest.permission.READ_EXTERNAL_STORAGE,
                        Manifest.permission.WRITE_EXTERNAL_STORAGE},
                123);
        surface = findViewById(R.id.surface);
        surface.getHolder().setFormat(PixelFormat.RGBA_8888);
    }

    public void init(View view) {
        File file = Environment.getExternalStorageDirectory();
        file = new File(file, "videos");
        file = new File(file, "1frame.h264");

        byte[] frame_buff_ = new byte[30882]; //这里是测试用的一帧大小 实际不用确定根据一帧大小来
        try {
            FileInputStream fis = new FileInputStream(file);
            if (fis.read(frame_buff_) == 30882) {
                Log.e(TAG, "readed ok:" + frame_buff_.length);
            }
            fis.close();
        } catch (Exception e) {
            e.printStackTrace();
        }

        JNIBridge.init(surface.getHolder().getSurface(), 25);
        int ret = JNIBridge.decode(frame_buff_);
        Log.e(TAG, "ret2:" + ret);
        JNIBridge.destory();

    }
}
