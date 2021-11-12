#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <sys/timeb.h>
#include <time.h>
#include <CL/opencl.h>
#include "ImageProcessing.h"

#define checkCL(expression)                                              \
    {                                                                    \
        cl_int err = (expression);                                       \
        if (err < 0 && err > -64)                                        \
        {                                                                \
            printf("Error on line %d. error code: %d\n", __LINE__, err); \
            exit(0);                                                     \
        }                                                                \
    }

int opencl_infra_creation(cl_context &context, cl_platform_id &cpPlatform, cl_device_id &device_id,
                          cl_command_queue &queue, cl_program &program, cl_kernel &kernel, char *kernel_file_buffer, size_t kernel_file_size,
                          BMPHEADER &bmpHeader, unsigned char *kernel_name, cl_mem &d_src, cl_mem &d_dst);

int launch_the_kernel(cl_context &context, cl_command_queue &queue, cl_kernel &kernel, size_t globalSize, size_t localSize, BMPHEADER &bmpHeader,
                      cl_mem &d_src, cl_mem &d_dst, unsigned char *image, unsigned char *blured_img);

int main(int argc, char *argv[])
{
    FILE *file_handle;
    char *file_buffer;
    size_t kernel_file_size;

    BMPHEADER bmpHeader;
    unsigned char *image, *blured_img;

    unsigned char *input_image_name = "lena.bmp";
    unsigned char *output_image_name = "lena.bmp";
    unsigned char *cl_file_name = "Blur.cl";
    unsigned char *kernel_name = "kernel_blur";

    cl_mem d_src;
    cl_mem d_dst;

    cl_platform_id cpPlatform;
    cl_device_id device_id;
    cl_context context;
    cl_command_queue queue;
    cl_program program;
    cl_kernel kernel;
    file_handle = fopen(cl_file_name, "r");
    if (file_handle == NULL)
    {
        printf("Couldn't find the file");
        exit(1);
    }

    // read kernel file
    fseek(file_handle, 0, SEEK_END);       // fseek: move pointer into the end of the File
    kernel_file_size = ftell(file_handle); // ftell: get the location of pointer
    rewind(file_handle);                   // rewind == fseek(filehanndle, 0, SEEK_SET): move pointer into the betin
                                           // of the File
    file_buffer = (char *)malloc(kernel_file_size + 1);
    file_buffer[kernel_file_size] = '\0';                            // the end of the string
    fread(file_buffer, sizeof(char), kernel_file_size, file_handle); // file text -> file_buffer string
    fclose(file_handle);

    image = read_bmp(inpupt_image_name, &bmpHeader);
    blured_img = (unsigned char *)malloc(bmpHeader.biSizeImage);

    int i;
    size_t globalSize, localSize, grid;

    localSize = 64;
    int n_pix = bmpHeader.biWidth * bmpHeader.biHeight;

    grid = n / localsize + (n_pix % localSize) ? 1 : 0;
    globalSize = grid * localSize;

    opencl_infra_creation(context, cpPlatform, device_id, queue, program, kernel, kernel_file_buffer,
                          kernel_file_size, bmpHeader, kernel_name, d_src, d_dst);
    launch_the_kernel(context, queue, kernel, globalSize, localSize, bmpHeader,
                      d_src, d_dst, image, blured_img);

    write_bmp(output_image_name, bmpHeader.biWidth, bmpHeader.biHeight, blured_img);

    checkCL(clReleaseMemObject(d_src));
    checkCL(clReleaseMemObject(d_dst));
    checkCL(clReleaseProgram(program));
    checkCL(clReleaseKernel(kernel));
    checkCL(clReleaseCommandQueue(queue));
    checkCL(clReleaseContext(context));

    fflush(stdout);
    free(image);
    free(blured_img);

    return 0;
}

int opencl_infra_creation(cl_context &context, cl_platform_id &cpPlatform, cl_device_id &device_id,
                          cl_command_queue &queue, cl_program &program, cl_kernel &kernel, char *kernel_file_buffer, size_t kernel_file_size,
                          BMPHEADER &bmpHeader, unsigned char *kernel_name, cl_mem &d_src, cl_mem &d_dst)
{
    cl_int err;
    checkCL(clGetPlatformIDs(1, &cpPlatform, NULL));
    checkCL(clGetDeviceIDs(cpPlatform, CL_DEVICE_TYPE_GPU, 1, &device_id, NULL));
    context = clCreateContext(0, 1, &device_id, NULL, NULL, &err);
    checkCL(err);
    queue = clCreateCommandQueue(context, device_id, 0, &err);
    checkCL(err);

    program = clCreateProgramWithSource(context, 1, (const char **)&kernel_file_buffer, &kernel_file_size, &err);
    checkCL(err);

    checkCL(clBuildProgram(program, 0, NULL, NULL, NULL, NULL));

    kernel = clCreateKernel(program, kernel_name, &err);
    checkCL(err);
    return 0;
}

int launch_the_kernel(cl_context &context, cl_command_queue &queue, cl_kernel &kernel, size_t globalSize, size_t localSize, BMPHEADER &bmpHeader,
                      cl_mem &d_src, cl_mem &d_dst, unsigned char *image, unsigned char *blured_img)
{
    cl_int err;
    size_t size = bmpHeader.biSizeImage;
    struct timeval start, end, timer;
    d_src = clCreateBuffer(context, CL_MEM_READ_ONLY, size, NULL, &err);
    checkCL(err);
    d_dst = clCreateBuffer(context, CL_MEM_WRITE_ONLY, size, NULL, &err);
    checkCL(err);
    checkCL(clEnqueueWriteBuffer(queue, d_src, CL_TRUE, 0, size, image, 0, NULL, NULL));

    checkCL(clSetKernelArg(kernel, 0, sizeof(cl_mem), &d_src));
    checkCL(clSetKernelArg(kernel, 1, sizeof(cl_mem), &d_dst));
    checkCL(clSetKernelArg(kernel, 2, sizeof(int), &bmpHeader.biWidth));
    checkCL(clSetKernelArg(kernel, 3, sizeof(int), &bmpHeader.biHeight));

    gettimeofday(&start, NULL); // timer1 begin
    checkCL(clEnqueueNDRangeKernel(queue, kernel, 1, NULL, &globalSize, &localSize, 0, NULL, NULL););

    checkCL(clFinish(queue));
    checkCL(clEnqueueReadBuffer(queue, d_dst, CL_TRUE, 0, bytes, blured_img, 0, NULL, NULL););

    gettimeofday(&end, NULL); // timer2 end
    timersub(&end, &start, &timer);
    printf("GPUtime: %lf   ", (timer.tv_usec / 1000.0 + timer.tv_sec * 1000.0));
    return 0;
}
