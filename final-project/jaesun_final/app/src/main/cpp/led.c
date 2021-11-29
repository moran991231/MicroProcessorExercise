#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <jni.h>

int fd = 0;

JNIEXPORT jint JNICALL
Java_com_mp_jaesun_1final_Led_openDriver(JNIEnv *env, jobject thiz, jstring path) {
    jboolean iscopy;
    const char *path_utf = (*env)->GetStringUTFChars(env, path, &iscopy);
    fd = open(path_utf, O_WRONLY);
    (*env)->ReleaseStringUTFChars(env, path, path_utf);
    if (fd < 0) return -1;
    else return 1;
}


JNIEXPORT void JNICALL
Java_com_mp_jaesun_1final_Led_closeDriver(JNIEnv *env, jobject thiz) {
    if (fd < 0) close(fd);
}

JNIEXPORT jint JNICALL
Java_com_mp_jaesun_1final_Led_writeDriver(JNIEnv *env, jobject thiz, jbyteArray data, jint length) {
    jbyte *chars = (*env)->GetByteArrayElements(env, data, 0);
    if (fd > 0) write(fd, (unsigned char *) chars, length);
    (*env)->ReleaseByteArrayElements(env, data, chars, 0);
}