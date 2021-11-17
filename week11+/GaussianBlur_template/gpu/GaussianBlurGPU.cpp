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
        if (err < 0 && err > -64)                                        \
        {                                                                \
            printf("Error on line %d. error code: %d\n", __LINE__, err); \
            exit(0);                                                     \
        }                                                                \
    }

void addCPU(float *a, float *b, float *r, int n);

int opencl_infra_creation(cl_context &context, cl_platform_id &cpPlatform, cl_device_id &device_id,
                          cl_command_queue &queue, cl_program &program, cl_kernel &kernel,
                          char *kernel_file_buffer, size_t kernel_file_size, BMPHEADER &bmpHeader,
                          unsigned char *kernel_name, cl_mem &d_src, cl_mem &d_dst);

int launch_the_kernel(cl_context &context, cl_command_queue &queue, cl_kernel &kernel,
                      size_t globalSize, size_t localSize, BMPHEADER &bmpHeader, cl_mem &d_src,
                      cl_mem &d_dst, unsigned char *image, unsigned char *blurred_img);

int main(int argc, char *argv[])
{
    FILE *file_handle;
    char *kernel_file_buffer, *file_log;
    size_t kernel_file_size, log_size;
    struct timeval start, end, timer;

    BMPHEADER bmpHeader;
    unsigned char *image;
    unsigned char *blurred_img;

    char *input_image_name = "lena.bmp";
    char *output_image_name = "blurred_lena_gpu.bmp";
    char *cl_file_name = "Blur.cl";
    char *kernel_name = "kernel_blur";

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

    file_handle = fopen(cl_file_name, "r");
    if (file_handle == NULL)
    {
        printf("Couldn't find the file");
        exit(1);
    }

    // read kernel file
    fseek(file_handle, 0, SEEK_END);
    kernel_file_size = ftell(file_handle);
    rewind(file_handle);
    kernel_file_buffer = (char *)malloc(kernel_file_size + 1);
    kernel_file_buffer[kernel_file_size] = '\0';
    fread(kernel_file_buffer, sizeof(char), kernel_file_size, file_handle);
    fclose(file_handle);

    // read image to processing
    image = read_bmp(input_image_name, &bmpHeader);
    blurred_img = (unsigned char *)malloc(bmpHeader.biSizeImage);

    // Initialize vectors on host
    int i;

    size_t globalSize, localSize, grid;

    // Number of work items in each local work group
    localSize = 64;
    int n_pix = bmpHeader.biWidth * bmpHeader.biHeight;

    // Number of total work items - localSize must be devisor
    grid = n_pix / localSize + ((n_pix % localSize) ? 1 : 0); // TODO
    globalSize = grid * localSize;                            // TODO
    opencl_infra_creation(context, cpPlatform, device_id, queue, program, kernel,
                          kernel_file_buffer, kernel_file_size, bmpHeader, kernel_name, d_src,
                          d_dst);

    launch_the_kernel(context, queue, kernel, globalSize, localSize, bmpHeader, d_src, d_dst, image,
                      blurred_img);

    write_bmp(output_image_name, bmpHeader.biWidth, bmpHeader.biHeight, blurred_img); // TODO

    // release OpenCL resources
    checkCL(clReleaseMemObject(d_src));
    checkCL(clReleaseMemObject(d_dst));
    checkCL(clReleaseProgram(program));
    checkCL(clReleaseKernel(kernel));
    checkCL(clReleaseCommandQueue(queue));
    checkCL(clReleaseContext(context));

    // release host memory
    free(image);
    free(blurred_img);

    return 0;
}

int opencl_infra_creation(cl_context &context, cl_platform_id &cpPlatform, cl_device_id &device_id,
                          cl_command_queue &queue, cl_program &program, cl_kernel &kernel,
                          char *kernel_file_buffer, size_t kernel_file_size, BMPHEADER &bmpHeader,
                          unsigned char *kernel_name, cl_mem &d_src, cl_mem &d_dst)
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
                      size_t globalSize, size_t localSize, BMPHEADER &bmpHeader, cl_mem &d_src,
                      cl_mem &d_dst, unsigned char *image, unsigned char *blurred_img)
{
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
    checkCL(clEnqueueReadBuffer(queue, d_dst, CL_TRUE, 0, bmpHeader.biSizeImage, blurred_img, 0,
                                NULL, NULL));
    gettimeofday(&end, NULL);

    timersub(&end, &start, &timer);
    printf("GPUtime : %lf\n", (timer.tv_usec / 1000.0 + timer.tv_sec * 1000.0));

    return 0;
}
