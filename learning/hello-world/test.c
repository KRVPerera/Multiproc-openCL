#define _CRT_SECURE_NO_WARNINGS
#define PROGRAM_FILE "hello_world.cl"
#define KERNEL_NAME "hello_world"

#include <stdio.h>
#include <stdlib.h>
#ifdef MAC
#include <OpenCL/cl.h>
#else
#define CL_USE_DEPRECATED_OPENCL_2_0_APIS
//#define CL_USE_DEPRECATED_OPENCL_1_2_APIS
#define CL_TARGET_OPENCL_VERSION 220
#include <CL/cl.h>
#endif

int main() {

    cl_platform_id platform;
    cl_device_id device;
    cl_int err;

    cl_context context;
    cl_kernel kernel;
    cl_command_queue queue;

    cl_program program;
    FILE *program_handle;
    char *program_buffer;
    char *program_log;
    size_t program_size;
    size_t log_size;

    err = clGetPlatformIDs(1, &platform, NULL);
    if (err < 0) {
        perror("Error: clGetPlatformIDs");
        exit(1);
    }

    err = clGetDeviceIDs(platform, CL_DEVICE_TYPE_ALL, 1, &device, NULL);
    if (err < 0) {
        perror("Error: clGetDeviceIDs");
        exit(1);
    }

    char buf[128];

    clGetDeviceInfo(device, CL_DEVICE_NAME, 128, buf, NULL);
    fprintf(stdout, "Device: %s\n", buf);

    context = clCreateContext(NULL, 1, &device, NULL, NULL, &err);
    if (err < 0) {
        perror("Error: clCreateContext");
        exit(1);
    }

    program_handle = fopen(PROGRAM_FILE, "r");
    if(program_handle == NULL) {
        perror("Couldn't find the program file");
        exit(1);   
    }
    fseek(program_handle, 0, SEEK_END);
    program_size = ftell(program_handle);
    rewind(program_handle);
    program_buffer = (char*)malloc(program_size+1);
    program_buffer[program_size] = '\0';
    fread(program_buffer, sizeof(char), program_size, program_handle);
    fclose(program_handle);

    printf("Programmer read OK\n");

    program = clCreateProgramWithSource(context, 1, (const char**)&program_buffer, &program_size, &err);
    if(err < 0) {
        perror("Error: clCreateProgramWithSource");
        exit(1);   
    }

    printf("clCreateProgramWithSource OK\n");

    free(program_buffer);

    err = clBuildProgram(program, 1, &device, NULL, NULL, NULL);
    if (err < 0) {
        clGetProgramBuildInfo(program, device, CL_PROGRAM_BUILD_LOG, 0, NULL, &log_size);
        program_log = (char*) malloc(log_size + 1);
        program_log[log_size] = '\0';
        clGetProgramBuildInfo(program, device, CL_PROGRAM_BUILD_LOG, log_size + 1, program_log, NULL);
        printf("%s\n", program_log);
        free(program_log);
        exit(1);
    }

    printf("clBuildProgram OK\n");

    kernel = clCreateKernel(program, KERNEL_NAME, &err);
    if(err < 0) {
        perror("Error: clCreateKernel");
        exit(1);   
    }

    char msg[16];
    cl_mem msg_buffer;

    msg_buffer = clCreateBuffer(context, CL_MEM_WRITE_ONLY, sizeof(msg), NULL, &err);
    if (err < 0) {
        perror("Error: clCreateBuffer");
        exit(1);
    }

    err = clSetKernelArg(kernel, 0, sizeof(cl_mem), &msg_buffer);
    if(err < 0) {
        perror("Error: clSetKernelArg");
        exit(1);   
    }

    queue = clCreateCommandQueue(context, device, 0, &err);
    if(err < 0) {
        perror("Error: clCreateCommandQueue");
        exit(1);   
    }

    err = clEnqueueTask(queue, kernel, 0, NULL, NULL);
    if(err < 0) {
        perror("Error: clEnqueueTask");
        exit(1);   
    }

    err = clEnqueueReadBuffer(queue, msg_buffer, CL_TRUE, 0, sizeof(msg), &msg, 0, NULL, NULL);
    if(err < 0) {
        perror("Error: clEnqueueReadBuffer");
        exit(1);   
    }

    printf("Kernel output: %s\n", msg);

    clReleaseMemObject(msg_buffer);
    clReleaseKernel(kernel);
    clReleaseCommandQueue(queue);
    clReleaseProgram(program);
    clReleaseContext(context);
    return 0;
}