
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <jni.h>

int fd=0;


JNIEXPORT jint JNICALL
Java_com_example_led_1example_MainActivity_openDriver(JNIEnv
* env,
jclass clazz, jstring
path) {
    jboolean iscopy;
    const char *path_utf = (*env)->GetStringUTFChars(env, path, &iscopy);
    fd = open(path_utf, O_WRONLY);
    (*env)->ReleaseStringUTFChars(env, path, path_utf);
    if(fd<0) return -1;
    else return 1;
}

JNIEXPORT void JNICALL
Java_com_example_led_1example_MainActivity_closeDriver(JNIEnv
* env,
jclass clazz
) {
    if(fd<0) close(fd);
}

JNIEXPORT void JNICALL
Java_com_example_led_1example_MainActivity_writeDriver(JNIEnv
* env,
jclass clazz, jbyteArray
data,
jint length
) {
    jbyte* chars = (*env)->GetByteArrayElements(env, data, 0);
    if(fd>0) write(fd, (unsigned char*)chars, length);
    char temp[8] = {0};
    for(int i=0; i<8; i++)
        temp[i]  = chars[i];
(*env)-> ReleaseByteArrayElements(env, data, chars,0);
}