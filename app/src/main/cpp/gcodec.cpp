#include <jni.h>
#include <string>

#include <android/log.h>
#include <android/native_window_jni.h>

extern "C" {
#include "libavformat/avformat.h" //封装格式上下文
#include "libavcodec/avcodec.h" //解码库
#include "libswscale/swscale.h"
#include "libswresample/swresample.h"
#include <libavutil/imgutils.h>
#include "libyuv.h"
}

#define LOGE(FORMAT, ...) __android_log_print(ANDROID_LOG_ERROR,"Gloomy",FORMAT,##__VA_ARGS__);

int _IS_REGISTER_ALL = 0;

bool is_inited = false;

AVCodec *h264_codec = NULL;
AVCodecContext *h264_codec_ctx = NULL;
ANativeWindow *native_window = NULL;

extern "C"
JNIEXPORT jint JNICALL
Java_com_gloomyer_h264_jni_JNIBridge_init(JNIEnv *env, jclass type, jobject surface, jint den) {

    if (is_inited) return 1;

    if (!_IS_REGISTER_ALL) {
        av_register_all();
        _IS_REGISTER_ALL = 1;
    }

    h264_codec = avcodec_find_decoder(AV_CODEC_ID_H264);
    if (!h264_codec) {
        return -1;
    }
    h264_codec_ctx = avcodec_alloc_context3(h264_codec);
    if (!h264_codec_ctx) {
        return -2;
    }

    h264_codec_ctx->time_base.num = 1;
    h264_codec_ctx->frame_number = 1;
    h264_codec_ctx->codec_type = AVMEDIA_TYPE_VIDEO;
    h264_codec_ctx->bit_rate = 0;
    h264_codec_ctx->time_base.den = den; //帧率

    if (avcodec_open2(h264_codec_ctx, h264_codec, 0) == 0) {
        native_window = ANativeWindow_fromSurface(env, surface);
        is_inited = true;
        return 0;
    }
    return -3;
}

extern "C"
JNIEXPORT jint JNICALL
Java_com_gloomyer_h264_jni_JNIBridge_decode(JNIEnv *env, jclass type,
                                            jbyteArray h264buff_) {
    int ret = 1;
    jbyte *h264buff = env->GetByteArrayElements(h264buff_, 0);

    AVPacket packet = {0};
    packet.data = reinterpret_cast<uint8_t *>(h264buff);
    packet.size = env->GetArrayLength(h264buff_);

    if (avcodec_send_packet(h264_codec_ctx, &packet) == 0) {
        AVFrame *yuv_frame = av_frame_alloc();
        AVFrame *rgb_frame = av_frame_alloc();

        if (avcodec_receive_frame(h264_codec_ctx, yuv_frame) == 0) {
            int height = h264_codec_ctx->height;
            int width = h264_codec_ctx->width;


            ANativeWindow_setBuffersGeometry(native_window, width, height,
                                             WINDOW_FORMAT_RGBA_8888);
            ANativeWindow_Buffer out_buffer;
            ANativeWindow_lock(native_window, &out_buffer, 0);
            av_image_fill_arrays(rgb_frame->data, rgb_frame->linesize,
                                 static_cast<const uint8_t *>(out_buffer.bits), AV_PIX_FMT_RGBA,
                                 width, height, 1);

            libyuv::I420ToARGB(yuv_frame->data[0], yuv_frame->linesize[0],
                               yuv_frame->data[2], yuv_frame->linesize[2],
                               yuv_frame->data[1], yuv_frame->linesize[1],
                               rgb_frame->data[0], rgb_frame->linesize[0],
                               width, height);

            ANativeWindow_unlockAndPost(native_window);
            ret = 0;
        }

        av_frame_free(&yuv_frame);
        av_frame_free(&rgb_frame);
    } else {
        ret = -1;
    }

    env->ReleaseByteArrayElements(h264buff_, h264buff, 0);
    return ret;
}

extern "C"
JNIEXPORT void JNICALL
Java_com_gloomyer_h264_jni_JNIBridge_destory(JNIEnv *env, jclass type) {
    if (!is_inited) return;
    ANativeWindow_release(native_window);
    avcodec_close(h264_codec_ctx);
    is_inited = false;
}