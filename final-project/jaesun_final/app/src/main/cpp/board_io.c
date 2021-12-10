#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <jni.h>
#include <android/log.h>


#define LOG_TAG "MY_BOARD"
#define LOGD(...) __android_log_print(ANDROID_LOG_DEBUG, LOG_TAG, __VA_ARGS__)
JNIEXPORT jint JNICALL
Java_com_mp_jaesun_1final_ioDevices_BoardIO_open(JNIEnv *env, jclass clazz, jstring path, jint option) {
    const char * path_utf = (*env)->GetStringUTFChars(env, path, NULL);
    int fd = open(path_utf,option);
    (*env)->ReleaseStringUTFChars(env, path, path_utf);
    return fd;
}

JNIEXPORT void JNICALL
Java_com_mp_jaesun_1final_ioDevices_BoardIO_close(JNIEnv *env, jclass clazz, jint fd) {
    if(fd>0) close(fd);
}

JNIEXPORT jint JNICALL
Java_com_mp_jaesun_1final_ioDevices_BoardIO_getInterrupt(JNIEnv *env, jclass clazz, jint fd) {
    int ret=0;
    char value[100];
    ret = read(fd, &value,100);
    if(ret<0) return -1;
    switch(value[0]){
        case 'U':
            return 1;
        case'D':
            return 2;
        case 'L':
            return 3;
        case 'R':
            return 4;
        case 'C':
            return 5;
        default:
            return 0;
    }
    return 0;
}

JNIEXPORT void JNICALL
Java_com_mp_jaesun_1final_ioDevices_BoardIO_write(JNIEnv *env, jclass clazz, jint fd, jbyteArray arr,
                                        jint len, jint time) {
    jbyte *chars = (*env)->GetByteArrayElements(env,arr,0);

    if(fd>0){
        int i;
        for(i=0; i<time; i++)
            write(fd, (unsigned char *) chars, len);
    }
    (*env)->ReleaseByteArrayElements(env, arr, chars, 0);
}
