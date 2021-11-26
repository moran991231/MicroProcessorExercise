#include <jni.h>
#include <stdio.h>
#include <stdlib.h>
//#inclde <sys/time.h>
#include <android/log.h>
#include <android/bitmap.h>
#include <jni.h>
#include <string.h>
#include <CL/opencl.h>

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
#define CL_FILE_NAME "/data/local/tmp/Blur.cl"


void gaussian_blur(unsigned char *src_img, unsigned char *dst_img, int width, int height, int ch);
int opencl_infra_creation(cl_context &context, cl_platform_id &cpPlatform, cl_device_id &device_id,
                          cl_command_queue &queue, cl_program &program, cl_kernel &kernel,
                          char *kernel_file_buffer, size_t kernel_file_size, char *kernel_name);
int launch_the_kernel(cl_context &context, cl_command_queue &queue, cl_kernel &kernel,
                      size_t globalSize, size_t localSize, AndroidBitmapInfo &info, cl_mem &d_src,
                      cl_mem &d_dst, unsigned char *image, unsigned char *blurred_img);
extern "C"
JNIEXPORT jobject
JNICALL
Java_com_example_gaussian_1blur_1jni_MainActivity_GaussianBlurBitmap(JNIEnv *env, jobject thiz,
                                                                     jobject bitmap) {
    AndroidBitmapInfo info;
    int ret;
    if ((ret = AndroidBitmap_getInfo(env, bitmap, &info)) < 0) {
        LOGE("AndroidBitmap_getInfo() failed! error=%d", ret);
        return NULL;
    }
    LOGD("w: %d, h: %d, stride: %d", info.width, info.height, info.stride);
    if (info.format != ANDROID_BITMAP_FORMAT_RGBA_8888) {
        LOGE("bitmap format is not RGBA_8888!");
        return NULL;
    }

    LOGD("reading bitmap pixels...");
    void *bitmapPixels;
    if ((ret = AndroidBitmap_lockPixels(env, bitmap, &bitmapPixels)) < 0) {
        LOGE("AndroidBitmap_lockPixels() failed! error= %d", ret);
        return NULL;
    }

    unsigned char* src = (unsigned char*) bitmapPixels;
    unsigned char *tempPixels = (unsigned char*) malloc(info.height * info.width * 4);
    int pixelsCount = info.height * info.width;
    memcpy(tempPixels, src, sizeof(uint32_t) * pixelsCount);

    gaussian_blur(tempPixels, src, info.width, info.height, 4);
    AndroidBitmap_unlockPixels(env, bitmap);
    free(tempPixels);
    return bitmap;

}
extern "C"
JNIEXPORT jobject JNICALL
Java_com_example_gaussian_1blur_1jni_MainActivity_GaussianBlurBitmapGpu(JNIEnv *env, jobject thiz, jobject bitmap) {
    AndroidBitmapInfo info;
    int ret;
    if ((ret = AndroidBitmap_getInfo(env, bitmap, &info)) < 0) {
        LOGE("AndroidBitmap_getInfo() failed! error=%d", ret);
        return NULL;
    }
    LOGD("w: %d, h: %d, stride: %d", info.width, info.height, info.stride);
    if (info.format != ANDROID_BITMAP_FORMAT_RGBA_8888) {
        LOGE("bitmap format is not RGBA_8888!");
        return NULL;
    }

    LOGD("reading bitmap pixels...");
    void *bitmapPixels;
    if ((ret = AndroidBitmap_lockPixels(env, bitmap, &bitmapPixels)) < 0) {
        LOGE("AndroidBitmap_lockPixels() failed! error= %d", ret);
        return NULL;
    }

    unsigned char* blurred_img = (unsigned char*) bitmapPixels;
    unsigned char *orig_img = (unsigned char*) malloc(info.height * info.width * 4);
    char *kernel_name = "kernel_blur_rgba8888";
    int pixelsCount = info.height * info.width;
    memcpy(orig_img, blurred_img, sizeof(uint32_t) * pixelsCount);

    //gpu

    // Device input buffers
    cl_mem d_src;
    // Device output buffer
    cl_mem d_dst;

    cl_platform_id cpPlatform; // OpenCL platform
    cl_device_id device_id;    // device ID
    cl_context context;        // context
    cl_command_queue queue;    // command queue
    cl_program program;        // program
    cl_kernel kernel;          // kernel

    FILE *file_handle=fopen(CL_FILE_NAME, "r");
    if (file_handle == NULL)
    {
        LOGD("Couldn't find the file");
        exit(1);
    }
    // read kernel file
    fseek(file_handle, 0, SEEK_END);
    size_t kernel_file_size = ftell(file_handle);
    rewind(file_handle);
    char* kernel_file_buffer = (char *)malloc(kernel_file_size + 1);
    kernel_file_buffer[kernel_file_size] = '\0';
    fread(kernel_file_buffer, sizeof(char), kernel_file_size, file_handle);
    fclose(file_handle);

    size_t globalSize, localSize, grid;

    // Number of work items in each local work group
    localSize = 64;
    int n_pix = info.width * info.height;

    // Number of total work items - localSize must be devisor
    grid = n_pix / localSize + ((n_pix % localSize) ? 1 : 0); // TODO
    globalSize = grid * localSize;                            // TODO

    opencl_infra_creation(context, cpPlatform, device_id, queue, program, kernel, kernel_file_buffer,
                          kernel_file_size, kernel_name);
    launch_the_kernel(context, queue, kernel, globalSize, localSize, info, d_src, d_dst, orig_img, blurred_img);

    checkCL(clReleaseMemObject(d_src));
    checkCL(clReleaseMemObject(d_dst));
    checkCL(clReleaseProgram(program));
    checkCL(clReleaseKernel(kernel));
    checkCL(clReleaseCommandQueue(queue));
    checkCL(clReleaseContext(context));
    //gpu end

    AndroidBitmap_unlockPixels(env, bitmap);
    free(orig_img);
    return bitmap;

}



void gaussian_blur(unsigned char *src_img, unsigned char *dst_img, int width, int height, int ch)
{
    float red = 0, green = 0, blue = 0;
    int row = 0, col = 0;
    int m, n, k;
    int pix;
    float mask[9][9] = {
            {0.011237, 0.011637, 0.011931, 0.012111, 0.012172, 0.012111, 0.011931, 0.011637, 0.011237},
            {0.011637, 0.012051, 0.012356, 0.012542, 0.012605, 0.012542, 0.012356, 0.012051, 0.011637},
            {0.011931, 0.012356, 0.012668, 0.012860, 0.012924, 0.012860, 0.012668, 0.012356, 0.011931},
            {0.012111, 0.012542, 0.012860, 0.013054, 0.013119, 0.013054, 0.012860, 0.012542, 0.012111},
            {0.012172, 0.012605, 0.012924, 0.013119, 0.013185, 0.013119, 0.012924, 0.012605, 0.012172},
            {0.012111, 0.012542, 0.012860, 0.013054, 0.013119, 0.013054, 0.012860, 0.012542, 0.012111},
            {0.011931, 0.012356, 0.012668, 0.012860, 0.012924, 0.012860, 0.012668, 0.012356, 0.011931},
            {0.011637, 0.012051, 0.012356, 0.012542, 0.012605, 0.012542, 0.012356, 0.012051, 0.011637},
            {0.011237, 0.011637, 0.011931, 0.012111, 0.012172, 0.012111, 0.011931, 0.011637, 0.011237}};

    // pixel for-loop (r:0~h, c:0~w)
    for (row = 0; row < height; row++)
    {
        for (col = 0; col < width; col++)
        {
            blue = green = red = 0;
            // gaussian matrix for-loop (m: 0~9, n: 0~9)
            for (m = 0; m < 9; m++)
            {
                for (n = 0; n < 9; n++)
                {
                    pix = (((row + m - 4) % height) * width) * ch + ((col + n - 4) % width) * ch;
                    red += src_img[pix + 0] * mask[m][n];
                    green += src_img[pix + 1] * mask[m][n];
                    blue += src_img[pix + 2] * mask[m][n];
                }
            }
            // set result
            dst_img[(row * width + col) * ch + 0] = red;
            dst_img[(row * width + col) * ch + 1] = green;
            dst_img[(row * width + col) * ch + 2] = blue;
        }
    }
}

int opencl_infra_creation(cl_context &context, cl_platform_id &cpPlatform, cl_device_id &device_id,
                          cl_command_queue &queue, cl_program &program, cl_kernel &kernel,
                          char *kernel_file_buffer, size_t kernel_file_size, char *kernel_name)
{
    cl_int err;

    checkCL(clGetPlatformIDs(1, &cpPlatform, NULL));                              // Bind to platform
    checkCL(clGetDeviceIDs(cpPlatform, CL_DEVICE_TYPE_GPU, 1, &device_id, NULL)); // Get ID for the device
    context = clCreateContext(0, 1, &device_id, NULL, NULL, &err);                // Create a context
    queue = clCreateCommandQueue(context, device_id, 0, &err);                    // Create a command queue
    checkCL(err);
    program = clCreateProgramWithSource(context, 1, (const char **)&kernel_file_buffer,
                                        &kernel_file_size, &err); // Create the compute program from the source buffer
    checkCL(err);
    checkCL(clBuildProgram(program, 0, NULL, NULL, NULL, NULL)); // Build the program executable
    kernel = clCreateKernel(program, kernel_name, &err);         // Create the compute kernel in the program we wish to run
    checkCL(err);

    return 0;
}

int launch_the_kernel(cl_context &context, cl_command_queue &queue, cl_kernel &kernel,
                      size_t globalSize, size_t localSize, AndroidBitmapInfo &info, cl_mem &d_src,
                      cl_mem &d_dst, unsigned char *image, unsigned char *blurred_img)
{
    cl_int err;
    size_t img_size = info.height*info.width*4;
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
    checkCL(clSetKernelArg(kernel, 3, sizeof(int),&(info.height)));


    // Execute the kernel over the entire range of the data set
    checkCL(clEnqueueNDRangeKernel(queue, kernel, 1, NULL, &globalSize, &localSize, 0, NULL, NULL));
    // Wait for the command queue to get serviced before reading back results
    checkCL(clFinish(queue));

    // Read the results from the device
    checkCL(clEnqueueReadBuffer(queue, d_dst, CL_TRUE, 0, img_size, blurred_img, 0,
                                NULL, NULL));


    return 0;
}
