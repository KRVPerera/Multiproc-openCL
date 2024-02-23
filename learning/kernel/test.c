#define _CRT_SECURE_NO_WARNINGS
#define PROGRAM_FILE "test.cl"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#ifdef MAC
#include <OpenCL/cl.h>
#else
#define CL_USE_DEPRECATED_OPENCL_2_0_APIS
#define CL_USE_DEPRECATED_OPENCL_1_2_APIS
#define CL_TARGET_OPENCL_VERSION 220
#include <CL/cl.h>
#endif

int kernelFound() {
    int kernelFound = 1;
    cl_platform_id platform;
    cl_device_id device;
    cl_context context;
    cl_int err;
    cl_uint i;

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

    program = clCreateProgramWithSource(context, 1, (const char**)&program_buffer, &program_size, &err);
    if(err < 0) {
        perror("Error: clCreateProgramWithSource");
        exit(1);
    }

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

    cl_kernel *kernels;
    cl_uint num_kernels;
    char kernel_name[20];

    err = clCreateKernelsInProgram(program, 0, NULL, &num_kernels);
    if(err < 0) {
        perror("Error: clCreateKernelsInProgram");
        exit(1);
    }

    kernels = (cl_kernel*) malloc(sizeof(cl_kernel) * num_kernels);
    clCreateKernelsInProgram(program, num_kernels, kernels, NULL);

    for (i = 0; i < num_kernels; i++) {
        clGetKernelInfo(kernels[i], CL_KERNEL_FUNCTION_NAME, 20, kernel_name, NULL);
        if (strcmp(kernel_name, "sub") == 0) {
            printf("Found the kernel at index %u\n", i);
            kernelFound = 0;
            break;
        }
    }

    for(i = 0; i < num_kernels; i++) {
        clReleaseKernel(kernels[i]);
    }

    free(kernels);

    clReleaseProgram(program);
    clReleaseContext(context);
    return kernelFound;
}

//int main() {
//    return kernelFound();
//}