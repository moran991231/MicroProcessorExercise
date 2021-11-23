#include <CL/opencl.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include <time.h>

#include "ImageProcessing.h"

#define checkCL(expression)                                              \
    {                                                                    \
        cl_int err = (expression);                                       \
        if (err < 0 && err > -64) {                                      \
            printf("Error on line %d. error code: %d\n", __LINE__, err); \
            exit(0);                                                     \
        }                                                                \
    }

char *read_kernel_file(char *cl_file_name, size_t &kernel_file_size);

int opencl_infra_creation(cl_context &context, cl_command_queue &queue, cl_program &program,
                          cl_kernel &kernel, char *kernel_file_buffer, size_t kernel_file_size,
                          char *kernel_name);

int launch_the_kernel(cl_context &context, cl_command_queue &queue, cl_kernel &kernel,
                      size_t globalSize, size_t localSize, BMPHEADER &bmpHeader, cl_mem &d_src,
                      cl_mem &d_dst, unsigned char *image, unsigned char *ret_img);
void getOneCh(unsigned char *src, unsigned char *dst, int w, int h, int ch);
int main(int argc, char *argv[]) {
    if (argc != 3) {
        printf("./rgb2hsv [input file name] [output file name]\n");
        return 0;
    }

    char *input_image_name = argv[1], *output_image_name = argv[2];

    printf("%s %s \n", input_image_name, output_image_name);
    // read kernel file
    size_t kernel_file_size;
    char *cl_file_name = "kernel_hsv.cl";
    char *kernel_file_buffer = read_kernel_file(cl_file_name, kernel_file_size);

    // read image to processing
    BMPHEADER bmpHeader;
    unsigned char *image, *ret_img;
    image = (unsigned char *)read_bmp(input_image_name, &bmpHeader);
    ret_img = (unsigned char *)malloc(bmpHeader.biSizeImage);
    int w = bmpHeader.biWidth, h = bmpHeader.biHeight;
    printf("w: %d  h: %d \n", w, h);

    // set kernerl name
    char *kernel_name = "kernel_rgb2hsv";

    // Device input buffers
    cl_mem d_src;
    // Device output buffer
    cl_mem d_dst;

    cl_context context;      // context
    cl_command_queue queue;  // command queue
    cl_program program;      // program
    cl_kernel kernel;        // kernel

    size_t globalSize, localSize, grid;

    // Number of work items in each local work group
    localSize = 64;
    int n_pix = w * h;

    // Number of total work items - localSize must be devisor
    grid = n_pix / localSize + ((n_pix % localSize) ? 1 : 0);
    globalSize = grid * localSize;
    opencl_infra_creation(context, queue, program, kernel, kernel_file_buffer, kernel_file_size,
                          kernel_name);

    launch_the_kernel(context, queue, kernel, globalSize, localSize, bmpHeader, d_src, d_dst, image,
                      ret_img);

    write_bmp(output_image_name, w, h, (char *)ret_img);
    getOneCh(ret_img, image, w, h, 0);
    write_bmp("ch0.bmp", w, h, (char *)image);
    getOneCh(ret_img, image, w, h, 1);
    write_bmp("ch1.bmp", w, h, (char *)image);
    getOneCh(ret_img, image, w, h, 2);
    write_bmp("ch2.bmp", w, h, (char *)image);

    printf("finish!\n");
    fflush(stdout);

    // release OpenCL resources
    checkCL(clReleaseMemObject(d_src));
    checkCL(clReleaseMemObject(d_dst));
    checkCL(clReleaseProgram(program));
    checkCL(clReleaseKernel(kernel));
    checkCL(clReleaseCommandQueue(queue));
    checkCL(clReleaseContext(context));

    // release host memory
    free(image);
    free(ret_img);

    return 0;
}
void getOneCh(unsigned char *src, unsigned char *dst, int w, int h, int ch) {
    unsigned char color;
    int i, j, pix;
    int hist[256] = {0};
    for (i = 0; i < h; i < i++) {
        for (j = 0; j < w; j++) {
            pix = (i * w + j) * 3;
            color = src[pix + ch];
            dst[pix + 0] = dst[pix + 1] = dst[pix + 2] = color;
            if (ch == 0) hist[color]++;
        }
    }
    if (ch == 0)
        for (i = 0; i < 256; i++) printf("%3d: %6d \n", i, hist[i]);
}
int opencl_infra_creation(cl_context &context, cl_command_queue &queue, cl_program &program,
                          cl_kernel &kernel, char *kernel_file_buffer, size_t kernel_file_size,
                          char *kernel_name) {
    cl_int err;
    cl_platform_id cpPlatform;  // OpenCL platform
    cl_device_id device_id;     // device ID
    // Bind to platform
    checkCL(clGetPlatformIDs(1, &cpPlatform, NULL));

    // Get ID for the device
    checkCL(clGetDeviceIDs(cpPlatform, CL_DEVICE_TYPE_GPU, 1, &device_id, NULL));

    // Create a context
    context = clCreateContext(0, 1, &device_id, NULL, NULL, &err);

    // Create a command queue
    queue = clCreateCommandQueue(context, device_id, 0, &err);
    checkCL(err);

    // Create the compute program from the source buffer
    program = clCreateProgramWithSource(context, 1, (const char **)&kernel_file_buffer,
                                        &kernel_file_size, &err);
    checkCL(err);

    // Build the program executable
    checkCL(clBuildProgram(program, 0, NULL, NULL, NULL, NULL));

    // Create the compute kernel in the program we wish to run
    kernel = clCreateKernel(program, kernel_name, &err);
    checkCL(err);

    return 0;
}

int launch_the_kernel(cl_context &context, cl_command_queue &queue, cl_kernel &kernel,
                      size_t globalSize, size_t localSize, BMPHEADER &bmpHeader, cl_mem &d_src,
                      cl_mem &d_dst, unsigned char *image, unsigned char *ret_img) {
    cl_int err;
    struct timeval start, end, timer;

    // Create the input and output arrays in device memory for our calculation
    d_src = clCreateBuffer(context, CL_MEM_READ_ONLY, bmpHeader.biSizeImage, NULL, &err);
    checkCL(err);
    d_dst = clCreateBuffer(context, CL_MEM_WRITE_ONLY, bmpHeader.biSizeImage, NULL, &err);
    checkCL(err);

    // Write our data set into the input array in device memory
    checkCL(clEnqueueWriteBuffer(queue, d_src, CL_TRUE, 0, bmpHeader.biSizeImage, image, 0, NULL,
                                 NULL));

    // Set the arguments to our compute kernel
    checkCL(clSetKernelArg(kernel, 0, sizeof(cl_mem), &d_src));
    checkCL(clSetKernelArg(kernel, 1, sizeof(cl_mem), &d_dst));
    checkCL(clSetKernelArg(kernel, 2, sizeof(int), &bmpHeader.biWidth));
    checkCL(clSetKernelArg(kernel, 3, sizeof(int), &bmpHeader.biHeight));

    gettimeofday(&start, NULL);

    // Execute the kernel over the entire range of the data set
    checkCL(clEnqueueNDRangeKernel(queue, kernel, 1, NULL, &globalSize, &localSize, 0, NULL, NULL));
    // Wait for the command queue to get serviced before reading back results
    checkCL(clFinish(queue));

    // Read the results from the device
    checkCL(clEnqueueReadBuffer(queue, d_dst, CL_TRUE, 0, bmpHeader.biSizeImage, ret_img, 0, NULL,
                                NULL));
    gettimeofday(&end, NULL);

    timersub(&end, &start, &timer);
    printf("GPUtime : %lf\n", (timer.tv_usec / 1000.0 + timer.tv_sec * 1000.0));

    return 0;
}

char *read_kernel_file(char *cl_file_name, size_t &kernel_file_size) {
    FILE *file_handle = fopen(cl_file_name, "r");
    if (file_handle == NULL) {
        printf("Couldn't find the file");
        exit(1);
    }
    // read kernel file
    fseek(file_handle, 0, SEEK_END);
    kernel_file_size = ftell(file_handle);
    rewind(file_handle);
    char *kernel_file_buffer = (char *)malloc(kernel_file_size + 1);
    kernel_file_buffer[kernel_file_size] = '\0';
    fread(kernel_file_buffer, sizeof(char), kernel_file_size, file_handle);
    fclose(file_handle);
    return kernel_file_buffer;
}
