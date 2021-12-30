#include <jni.h>
#include <stdio.h>
#include <stdlib.h>
#include <android/log.h>
#include <android/bitmap.h>
#include <jni.h>
#include <string.h>
#include <CL/opencl.h>
#include <jni.h>

#define LOG_TAG "MY_LOG"
#define LOGD(...) __android_log_print(ANDROID_LOG_DEBUG, LOG_TAG, __VA_ARGS__)
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, __VA_ARGS__)
#define checkCL(expression)                                              \
    {                                                                    \
        cl_int err = (expression);                                       \
        if (err < 0 && err > -64)                                        \
        {                                                                \
            LOGD("Error on line %d. error code: %d\n", __LINE__, err);   \
            exit(0);                                                     \
        }                                                                \
    }
#define LOCAL_SIZE (64U)

int opencl_infra_creation(cl_context &context, cl_platform_id &cpPlatform, cl_device_id &device_id,
                          cl_command_queue &queue, cl_program &program, cl_kernel &kernel,
                          char *kernel_file_buffer, size_t kernel_file_size, const char *kernel_name);

int launch_the_kernel(cl_context &context, cl_command_queue &queue, cl_kernel &kernel,
                      size_t globalSize, size_t localSize, AndroidBitmapInfo &info, cl_mem &d_src,
                      cl_mem &d_dst,cl_mem &d_th, unsigned char *image, unsigned char *ret_img, unsigned char* th) ;
int get_ready_bitmap(JNIEnv *env, jobject bitmap, AndroidBitmapInfo *info, unsigned char **tempPixels,
                 unsigned char **src) ;
void read_kernel_file(const char* kernel_file_name, size_t *kernel_file_size, char **kernel_file_buffer);
int release_bitmap(JNIEnv *env, jobject bitmap, unsigned char *tempPixels);
size_t calc_global_size(int total_size, int local_size);
unsigned char* get_uchar_array(JNIEnv*env, jbyteArray ranges);

extern "C"
JNIEXPORT jint JNICALL
Java_com_mp_jaesun_1final_helper_MyBitmap_rgb2hsv(JNIEnv *env, jobject thiz, jobject bitmap) {

    AndroidBitmapInfo info;
    unsigned char *ret_img, *orig_img;
    const char *kernel_name = "kernel_rgb2hsv";
    const char* kernel_file_name = "/data/local/tmp/hsv.c";
    size_t kernel_file_size;
    char *kernel_file_buffer;
    int ret;
    ret = get_ready_bitmap(env, bitmap, &info, &orig_img, &ret_img);
    if(ret<0) return -1;

    int pixelsCount = info.height * info.width;
    memcpy(orig_img, ret_img, sizeof(uint32_t) * pixelsCount);

    /////// gpu code begin
    cl_mem d_src, d_dst;
    cl_platform_id cpPlatform; // OpenCL platform
    cl_device_id device_id;    // device ID
    cl_context context;        // context
    cl_command_queue queue;    // command queue
    cl_program program;        // program
    cl_kernel kernel;          // kernel

    LOGD("gpu: cl file reading");
    read_kernel_file(kernel_file_name,&kernel_file_size, &kernel_file_buffer);

    size_t globalSize = calc_global_size(pixelsCount,LOCAL_SIZE);
    // get ready to launch kernel
    LOGD("gpu: getting ready to use kernel");
    opencl_infra_creation(context, cpPlatform, device_id, queue, program, kernel,
                          kernel_file_buffer,kernel_file_size, kernel_name);
    // launch kernel
    LOGD("gpu: launch the kernel");
    cl_mem x;
    launch_the_kernel(context, queue, kernel, globalSize, LOCAL_SIZE, info, d_src, d_dst,x, orig_img,
                      ret_img,NULL);
    // release
    LOGD("gpu: release resources");
    checkCL(clReleaseMemObject(d_src));
    checkCL(clReleaseMemObject(d_dst));
    checkCL(clReleaseProgram(program));
    checkCL(clReleaseKernel(kernel));
    checkCL(clReleaseCommandQueue(queue));
    checkCL(clReleaseContext(context));
    ////// gpu code ends

    release_bitmap(env, bitmap, orig_img);
    return 0;
}
extern "C"
JNIEXPORT jint JNICALL
Java_com_mp_jaesun_1final_helper_MyBitmap_inRange(JNIEnv *env, jobject thiz, jobject bitmap,
                                           jbyteArray ranges) {
    // translate input arguments (bitmap, ranges)
    unsigned char color_range[6]={0};
    jbyte *rawBytes = (*env).GetByteArrayElements(ranges,NULL);
    memcpy(color_range,rawBytes,sizeof(color_range));
    (*env).ReleaseByteArrayElements(ranges,rawBytes,0);
    AndroidBitmapInfo info;
    unsigned char *ret_img, *orig_img;
    const char *kernel_name = "kernel_in_range_";
    const char* kernel_file_name = "/data/local/tmp/threshold.c";
    size_t kernel_file_size;
    char *kernel_file_buffer;
    int ret;
    ret = get_ready_bitmap(env, bitmap, &info, &orig_img, &ret_img);
    if(ret<0) return -1;

    int pixelsCount = info.height * info.width;
    memcpy(orig_img, ret_img, sizeof(uint32_t) * pixelsCount);

    /////// gpu code begin
    cl_mem d_src, d_dst, d_th;
    cl_platform_id cpPlatform; // OpenCL platform
    cl_device_id device_id;    // device ID
    cl_context context;        // context
    cl_command_queue queue;    // command queue
    cl_program program;        // program
    cl_kernel kernel;          // kernel

    LOGD("gpu: cl file reading");
    read_kernel_file(kernel_file_name,&kernel_file_size, &kernel_file_buffer);

    size_t globalSize = calc_global_size(pixelsCount,LOCAL_SIZE);
    // get ready to launch kernel
    LOGD("gpu: getting ready to use kernel");
    opencl_infra_creation(context, cpPlatform, device_id, queue, program, kernel,
                          kernel_file_buffer,kernel_file_size, kernel_name);
    // launch kernel
    LOGD("gpu: launch the kernel");
    launch_the_kernel(context, queue, kernel, globalSize, LOCAL_SIZE, info, d_src, d_dst,d_th, orig_img,
                      ret_img,color_range);
    // release
    LOGD("gpu: release resources");
    checkCL(clReleaseMemObject(d_src));
    checkCL(clReleaseMemObject(d_dst));
    checkCL(clReleaseProgram(program));
    checkCL(clReleaseKernel(kernel));
    checkCL(clReleaseCommandQueue(queue));
    checkCL(clReleaseContext(context));
    ////// gpu code ends

    release_bitmap(env, bitmap, orig_img);
    free(color_range);
    return 0;
}

void read_kernel_file(const char* kernel_file_name, size_t *kernel_file_size, char **kernel_file_buffer) {
    FILE *file_handle = fopen(kernel_file_name, "r");
    if (file_handle == NULL) {
        LOGD("Couldn't find the file");
        exit(1);
    }
    // read kernel file
    fseek(file_handle, 0, SEEK_END);
    *kernel_file_size = ftell(file_handle);
    rewind(file_handle);
    *kernel_file_buffer = (char *) malloc((*kernel_file_size) + 1);
    (*kernel_file_buffer)[*kernel_file_size] = '\0';
    fread(*kernel_file_buffer, sizeof(char), *kernel_file_size, file_handle);
    fclose(file_handle);
}
int get_ready_bitmap(JNIEnv *env, jobject bitmap, AndroidBitmapInfo *info, unsigned char **tempPixels,
                     unsigned char **src) {
    int ret;
    if ((ret = AndroidBitmap_getInfo(env, bitmap, info)) < 0) {
        LOGE("AndroidBitmap_getInfo() failed! error=%d", ret);
        return -1;
    }
    LOGD("w: %d, h: %d, stride: %d", info->width, info->height, info->stride);
    if (info->format != ANDROID_BITMAP_FORMAT_RGBA_8888) {
        LOGE("bitmap format is not RGBA_8888!");
        return -1;
    }

    LOGD("reading bitmap pixels...");
    void *bitmapPixels;

    // grasp bitmap image exclusively
    if ((ret = AndroidBitmap_lockPixels(env, bitmap, &bitmapPixels)) < 0) {
        LOGE("AndroidBitmap_lockPixels() failed! error= %d", ret);
        return -1;
    }
    LOGD("make temp pixels...");
    // make temp image and copy orig image into temp image
    *src = (unsigned char *) bitmapPixels;
    *tempPixels = (unsigned char *) malloc((info->height) * (info->width) * 4);
    int pixelsCount = (info->height) * (info->width);
    LOGD("copy temp pixels...");
    memcpy(*tempPixels, *src, sizeof(uint32_t) * pixelsCount);
    LOGD("finished getting ready");
    return 0;
}
int release_bitmap(JNIEnv *env, jobject bitmap, unsigned char *tempPixels) {
    AndroidBitmap_unlockPixels(env, bitmap);
    free(tempPixels);
    return 0;
}


int opencl_infra_creation(cl_context &context, cl_platform_id &cpPlatform, cl_device_id &device_id,
                          cl_command_queue &queue, cl_program &program, cl_kernel &kernel,
                          char *kernel_file_buffer, size_t kernel_file_size, const char *kernel_name) {
    cl_int err;

    checkCL(clGetPlatformIDs(1, &cpPlatform,
                             NULL));                              // Bind to platform
    checkCL(clGetDeviceIDs(cpPlatform, CL_DEVICE_TYPE_GPU, 1, &device_id,
                           NULL)); // Get ID for the device
    context = clCreateContext(0, 1, &device_id, NULL, NULL,
                              &err);                // Create a context
    queue = clCreateCommandQueue(context, device_id, 0,
                                 &err);                    // Create a command queue
    checkCL(err);
    program = clCreateProgramWithSource(context, 1, (const char **) &kernel_file_buffer,
                                        &kernel_file_size,
                                        &err); // Create the compute program from the source buffer
    checkCL(err);
    checkCL(clBuildProgram(program, 0, NULL, NULL, NULL, NULL)); // Build the program executable
    kernel = clCreateKernel(program, kernel_name,
                            &err);         // Create the compute kernel in the program we wish to run
    checkCL(err);

    return 0;
}

int launch_the_kernel(cl_context &context, cl_command_queue &queue, cl_kernel &kernel,
                      size_t globalSize, size_t localSize, AndroidBitmapInfo &info, cl_mem &d_src,
                      cl_mem &d_dst,cl_mem &d_th, unsigned char *image, unsigned char *ret_img, unsigned char* th) {
    cl_int err;
    size_t img_size = info.height * info.width * 4;
    // Create the input and output arrays in device memory for our calculation
    d_src = clCreateBuffer(context, CL_MEM_READ_ONLY, img_size, NULL, &err);
    checkCL(err);
    d_dst = clCreateBuffer(context, CL_MEM_WRITE_ONLY, img_size, NULL, &err);
    checkCL(err);

    // Write our data set into the input array in device memory
    checkCL(clEnqueueWriteBuffer(queue, d_src, CL_TRUE, 0, img_size, image, 0, NULL,
                                 NULL));


    // Set the arguments to our compute kernel
    checkCL(clSetKernelArg(kernel, 0, sizeof(cl_mem), &d_src));
    checkCL(clSetKernelArg(kernel, 1, sizeof(cl_mem), &d_dst));
    checkCL(clSetKernelArg(kernel, 2, sizeof(int), &(info.width)));
    checkCL(clSetKernelArg(kernel, 3, sizeof(int), &(info.height)));


    if(th!=NULL){
        d_th = clCreateBuffer(context,CL_MEM_READ_ONLY, 6,NULL, &err);
        checkCL(err);
        checkCL(clEnqueueWriteBuffer(queue,d_th,CL_TRUE,0,6,th,0,NULL,NULL));
        checkCL(clSetKernelArg(kernel,4,sizeof(cl_mem), &d_th));
    }
    // Execute the kernel over the entire range of the data set
    checkCL(clEnqueueNDRangeKernel(queue, kernel, 1, NULL, &globalSize, &localSize, 0, NULL, NULL));
    // Wait for the command queue to get serviced before reading back results
    checkCL(clFinish(queue));

    // Read the results from the device
    checkCL(clEnqueueReadBuffer(queue, d_dst, CL_TRUE, 0, img_size, ret_img, 0,
                                NULL, NULL));


    return 0;
}

size_t calc_global_size(int total_size, int local_size){
    size_t globalSize,  grid;
    // Number of total work items - localSize must be devisor
    grid = total_size / LOCAL_SIZE + ((total_size % LOCAL_SIZE) ? 1 : 0);
    globalSize = grid * LOCAL_SIZE;
    return globalSize;
}

unsigned char* get_uchar_array(JNIEnv*env, jbyteArray ranges){
    size_t len = (*env).GetArrayLength(ranges);
    jbyte* jranges = (*env).GetByteArrayElements(ranges,0);
    unsigned char* ret = (unsigned char*)malloc(len);
    memcpy(ret,jranges,len);
    return ret;
}


