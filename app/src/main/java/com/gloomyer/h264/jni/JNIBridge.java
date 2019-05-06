package com.gloomyer.h264.jni;

import android.view.Surface;
import android.view.SurfaceView;

public class JNIBridge {
    static {
        System.loadLibrary("ffmpeg");
        System.loadLibrary("gcodec");
    }

    /**
     * 初始化
     * <p>
     *
     * @param surface surface
     * @param den     码率 一秒多少帧
     * @return 0成功 -1:解码器没找到，-2解码器上下文分配失败 -3解码器打开失败 1: 已经注册过了
     */
    public native static int init(Surface surface, int den);

    /**
     * 将h264数据 解码并且显示到初始化提供的Surface中
     *
     * @param h264buff h264一帧数据 必须保证一帧数据的完整，否则会乱码
     * @return 0：表示成功解码 -1表示失败
     */
    public native static int decode(byte[] h264buff);

    /**
     * 释放资源
     */
    public native static void destory();
}
