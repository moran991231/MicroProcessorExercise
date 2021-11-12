#define CL_FILE "Vadd.cl"

#include <CL/opencl.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include <time.h>

void addCPU(float *a, float *b, float *r, int n);
float compareResult(float *cpus, float *gpus, int n);
float compareResult2(float *cpus, float *gpus, int n);

int main(int argc, char *argv[]) {
    if (argc < 2) {  // unvalid argument
        puts("usage: Vadd [N] \n");
        return 0;
    }

    unsigned int n = atoi(argv[1]);
    FILE *file_handle;
    char *file_buffer, *file_log;
    size_t file_size, log_size;
    struct timeval start, end, timer;

    // Host input vectors
    float *h_a, *h_b;
    // Host output vector
    float *h_c, *h_c2;

    float temp = 0;

    cl_mem d_a, d_b, d_c;

    cl_platform_id cpPlatform;
    cl_device_id device_id;
    cl_context context;
    cl_command_queue queue;
    cl_program program;
    cl_kernel kernel;

    size_t bytes = n * sizeof(float);

    file_handle = fopen(CL_FILE, "r");
    if (file_handle == NULL) {
        printf("Couldn't find the file");
        exit(1);
    }

    // read kernel file
    fseek(file_handle, 0, SEEK_END);
    file_size = ftell(file_handle);
    rewind(file_handle);
    file_buffer = (char *)malloc(file_size + 1);
    file_buffer[file_size] = '\0';
    fread(file_buffer, sizeof(char), file_size, file_handle);
    fclose(file_handle);

    h_a = (float *)malloc(bytes);
    h_b = (float *)malloc(bytes);
    h_c = (float *)malloc(bytes);
    h_c2 = (float *)malloc(bytes);

    int i;
    srand(time(NULL));
    for (i = 0; i < n; i++) {
        h_a[i] = (float)rand() / RAND_MAX;
        h_b[i] = (float)rand() / RAND_MAX;
        h_c[i] = 0.0f;
        h_c2[i] = 0.0f;
    }

    size_t globalSize, localSize, grid;
    cl_int err;

    localSize = 64;

    grid = (n % localSize) ? (n / localSize) + 1 : n / localSize;
    globalSize = grid * localSize;

    err = clGetPlatformIDs(1, &cpPlatform, NULL);

    err = clGetDeviceIDs(cpPlatform, CL_DEVICE_TYPE_GPU, 1, &device_id, NULL);

    context = clCreateContext(0, 1, &device_id, NULL, NULL, &err);

    queue = clCreateCommandQueue(context, device_id, 0, &err);

    program = clCreateProgramWithSource(context, 1, (const char **)&file_buffer, &file_size, &err);

    clBuildProgram(program, 0, NULL, NULL, NULL, NULL);

    kernel = clCreateKernel(program, "vecAdd", &err);

    d_a = clCreateBuffer(context, CL_MEM_READ_ONLY, bytes, NULL, NULL);
    d_b = clCreateBuffer(context, CL_MEM_READ_ONLY, bytes, NULL, NULL);
    d_c = clCreateBuffer(context, CL_MEM_WRITE_ONLY, bytes, NULL, NULL);

    err = clEnqueueWriteBuffer(queue, d_a, CL_TRUE, 0, bytes, h_a, 0, NULL, NULL);
    err |= clEnqueueWriteBuffer(queue, d_b, CL_TRUE, 0, bytes, h_b, 0, NULL, NULL);

    err = clSetKernelArg(kernel, 0, sizeof(cl_mem), &d_a);
    err |= clSetKernelArg(kernel, 1, sizeof(cl_mem), &d_b);
    err |= clSetKernelArg(kernel, 2, sizeof(cl_mem), &d_c);
    err |= clSetKernelArg(kernel, 3, sizeof(unsigned int), &n);

    gettimeofday(&start, NULL);

    err = clEnqueueNDRangeKernel(queue, kernel, 1, NULL, &globalSize, &localSize, 0, NULL, NULL);

    clFinish(queue);
    clEnqueueReadBuffer(queue, d_c, CL_TRUE, 0, bytes, h_c, 0, NULL, NULL);

    gettimeofday(&end, NULL);
    timersub(&end, &start, &timer);
    printf("GPUtime: %lf   ", (timer.tv_usec / 1000.0 + timer.tv_sec * 1000.0));

    clReleaseMemObject(d_a);
    clReleaseMemObject(d_b);
    clReleaseMemObject(d_c);
    clReleaseProgram(program);
    clReleaseKernel(kernel);
    clReleaseCommandQueue(queue);
    clReleaseContext(context);

    gettimeofday(&start, NULL);
    addCPU(h_a, h_b, h_c2, n);

    gettimeofday(&end, NULL);
    timersub(&end, &start, &timer);
    printf("CPUtime: %lf \n", (timer.tv_usec / 1000.0 + timer.tv_sec * 1000.0));

    temp = compareResult2(h_c2, h_c, n);
    printf("compare result: %lf ", temp);
    if (temp == 0) {
        printf("SUCCEEDED!! \n");
    } else {
        printf("ERROR!! \n");
    }

    fflush(stdout);

    return 0;
}

void addCPU(float *a, float *b, float *r, int n) {
    int i = 0;
    for (i = 0; i < n; i++) r[i] = a[i] + b[i];
}
float compareResult(float *cpus, float *gpus, int n) {
    int i = 0;
    float ret = 0.0f, temp;
    for (i = 0; i < n; i++) {
        temp = cpus[i] - gpus[i];
        ret += (temp >= 0 ? temp : -temp);
    }
    return ret;
}

float compareResult2(float *cpus, float *gpus, int n) {
    int i = 0;
    for (i = 0; i < n; i++)
        if (cpus[i] != gpus[i]) return i + 1;
    return 0;
}