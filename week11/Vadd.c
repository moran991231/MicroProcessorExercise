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
    FILE *file_handle;  // CL_FILE (Vadd.cl)
    char *file_buffer;  // content of CL_FILE
    size_t file_size;
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
    fseek(file_handle, 0, SEEK_END);  // fseek: move pointer into the end of the File
    file_size = ftell(file_handle);   // ftell: get the location of pointer
    rewind(file_handle);  // rewind == fseek(filehanndle, 0, SEEK_SET): move pointer into the betin
                          // of the File
    file_buffer = (char *)malloc(file_size + 1);
    file_buffer[file_size] = '\0';                             // the end of the string
    fread(file_buffer, sizeof(char), file_size, file_handle);  // file text -> file_buffer string
    fclose(file_handle);                                       // close file (Vadd.cl)

    // make float array for calculating
    h_a = (float *)malloc(bytes);
    h_b = (float *)malloc(bytes);
    h_c = (float *)malloc(bytes);
    h_c2 = (float *)malloc(bytes);

    int i;
    srand(time(NULL));  // rand seed: time
    // fill h_a, h_b as random value 0~1
    // fill h_c, h_c2 as zero
    for (i = 0; i < n; i++) {
        h_a[i] = (float)rand() / RAND_MAX;
        h_b[i] = (float)rand() / RAND_MAX;
        h_c[i] = 0.0f;
        h_c2[i] = 0.0f;
    }

    size_t globalSize, localSize, grid;
    cl_int err;

    localSize = 64;  // size of work group

    grid = (n % localSize) ? (n / localSize) + 1 : n / localSize;
    globalSize = grid * localSize;  // (num of work group)*(size of work group)
    // n <= globalSize

    err = clGetPlatformIDs(1, &cpPlatform, NULL);  // obtain available platform
    err = clGetDeviceIDs(cpPlatform, CL_DEVICE_TYPE_GPU, 1, &device_id, NULL);

    context = clCreateContext(0, 1, &device_id, NULL, NULL, &err);  // openCL context from dev type

    queue = clCreateCommandQueue(context, device_id, 0, &err);  // 1 command queue

    // build .cl to execute on the device
    program = clCreateProgramWithSource(context, 1, (const char **)&file_buffer, &file_size, &err);
    clBuildProgram(program, 0, NULL, NULL, NULL, NULL);

    // get ready to give a job to device
    kernel = clCreateKernel(program, "vecAdd", &err);

    // three params needed: d_a(h_a), d_b(h_b), d_c(h_c)
    d_a = clCreateBuffer(context, CL_MEM_READ_ONLY, bytes, NULL, NULL);
    d_b = clCreateBuffer(context, CL_MEM_READ_ONLY, bytes, NULL, NULL);
    d_c = clCreateBuffer(context, CL_MEM_WRITE_ONLY, bytes, NULL, NULL);

    // copy h_x to d_x
    err = clEnqueueWriteBuffer(queue, d_a, CL_TRUE, 0, bytes, h_a, 0, NULL, NULL);
    err |= clEnqueueWriteBuffer(queue, d_b, CL_TRUE, 0, bytes, h_b, 0, NULL, NULL);

    // set d_x as params
    err = clSetKernelArg(kernel, 0, sizeof(cl_mem), &d_a);
    err |= clSetKernelArg(kernel, 1, sizeof(cl_mem), &d_b);
    err |= clSetKernelArg(kernel, 2, sizeof(cl_mem), &d_c);
    err |= clSetKernelArg(kernel, 3, sizeof(unsigned int), &n);

    /////// gpu ///////
    gettimeofday(&start, NULL);  // timer1 begin

    // work!!
    err = clEnqueueNDRangeKernel(queue, kernel, 1, NULL, &globalSize, &localSize, 0, NULL, NULL);

    clFinish(queue);                                                         // wait
    clEnqueueReadBuffer(queue, d_c, CL_TRUE, 0, bytes, h_c, 0, NULL, NULL);  // copy d_c to h_c

    gettimeofday(&end, NULL);  // timer2 end
    timersub(&end, &start, &timer);
    printf("GPUtime: %lf   ", (timer.tv_usec / 1000.0 + timer.tv_sec * 1000.0));

    // release resources
    clReleaseMemObject(d_a);
    clReleaseMemObject(d_b);
    clReleaseMemObject(d_c);
    clReleaseProgram(program);
    clReleaseKernel(kernel);
    clReleaseCommandQueue(queue);
    clReleaseContext(context);

    /////// cpu ///////
    gettimeofday(&start, NULL);  // begin timer
    addCPU(h_a, h_b, h_c2, n);   // work!!
    gettimeofday(&end, NULL);    // end timer
    timersub(&end, &start, &timer);
    printf("CPUtime: %lf \n", (timer.tv_usec / 1000.0 + timer.tv_sec * 1000.0));

    /////// check ///////
    temp = compareResult(h_c2, h_c, n);
    printf("error sum: %lf ", temp);
    if (temp == 0) {
        printf("SUCCEEDED!! \n");
    } else {
        printf("ERROR!! \n");
    }

    fflush(stdout);  // print all contens in buffer

    // release resources
    free(h_a);
    free(h_b);
    free(h_c);
    free(h_c2);

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