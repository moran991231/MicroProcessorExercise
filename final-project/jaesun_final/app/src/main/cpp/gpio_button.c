#include <jni.h>
#include <fcntl.h>
#include <string.h>
#include <unistd.h>
int fd=0;
JNIEXPORT jint JNICALL
Java_com_mp_jaesun_1final_GpioButton_openDriver(JNIEnv *env, jobject thiz, jstring path) {
    jboolean iscopy;
    const char * path_utf = (*env)->GetStringUTFChars(env,path,&iscopy);
    fd = open(path_utf,O_RDONLY);
    (*env)->ReleaseStringUTFChars(env, path, path_utf);
    return fd<0? -1:1;
}


JNIEXPORT void JNICALL
Java_com_mp_jaesun_1final_GpioButton_closeDriver(JNIEnv *env, jobject thiz) {
    if(fd>0) close(fd);
}



char* strings[5]={"Up", "Down","Left","Right","Center"};
JNIEXPORT jint JNICALL
Java_com_mp_jaesun_1final_GpioButton_getInterrupt(JNIEnv *env, jobject thiz) {
    int ret=0;
    char value[100];
    ret = read(fd, &value,100);
    if(ret<0) return -1;
    for(int i=0;i<5; i++)
        if(strcmp(value, strings[i])==0)
            return i+1;
    return 0;
}