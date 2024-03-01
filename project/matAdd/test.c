#define _CRT_SECURE_NO_WARNINGS
#define PROGRAM_FILE "matrix_add.cl"
#define KERNEL_NAME "matrix_add"
#include <util.h>

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <opencl_include.h>

#define SIZE 100

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

    cl_event prof_event;
    cl_ulong time_start, time_end, total_time;

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

    kernel = clCreateKernel(program, KERNEL_NAME, &err);
    if(err < 0) {
        perror("Error: clCreateKernel");
        exit(1);   
    }

    queue = clCreateCommandQueue(context, device, 0, &err);
    if(err < 0) {
        perror("Error: clCreateCommandQueue");
        exit(1);   
    }

    cl_mem mat1_buffer, mat2_buffer, result_buffer;

    // Write data to buffers
    float *A = (float *)malloc(sizeof(float) * SIZE * SIZE);
    float *B = (float *)malloc(sizeof(float) * SIZE * SIZE);
    float *correct = (float *)malloc(sizeof(float) * SIZE * SIZE);
    for (int i = 0; i < SIZE; i++) {
        for (int j = 0; j < SIZE; j++) {
            A[i * SIZE + j] = i * SIZE + j;
            B[i * SIZE + j] = (SIZE - i) * SIZE + (SIZE - j);
        }
    }

    struct timespec t0, t1;
    unsigned long sec, nsec;
    GET_TIME(t0);
    for (int i = 0; i < SIZE; i++) {
        for (int j = 0; j < SIZE; j++) {
            correct[i * SIZE + j] = A[i * SIZE + j] + B[i * SIZE + j];
        }
    }
    GET_TIME(t1);
    float elapsed_time = elapsed_time_microsec(&t0, &t1, &sec, &nsec);
    printf("CPU Time : %f micro seconds\n", elapsed_time);

    // Create buffers for matrices A, B, and C
    mat1_buffer = clCreateBuffer(context, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, sizeof(float) * SIZE * SIZE, A, &err);
    if (err < 0) {
        perror("Error: clCreateBuffer");
        exit(1);
    }

    mat2_buffer = clCreateBuffer(context, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, sizeof(float) * SIZE * SIZE, B, &err);
    result_buffer = clCreateBuffer(context, CL_MEM_WRITE_ONLY, sizeof(float) * SIZE * SIZE, NULL, &err);

    // Set kernel arguments
    err = clSetKernelArg(kernel, 0, sizeof(cl_mem), &mat1_buffer);
    if(err < 0) {
        perror("Error: clSetKernelArg");
        exit(1);   
    }
    clSetKernelArg(kernel, 1, sizeof(cl_mem), &mat2_buffer);
    clSetKernelArg(kernel, 2, sizeof(cl_mem), &result_buffer);

    // Execute the kernel
    size_t globalSize[2] = {SIZE, SIZE};
    err = clEnqueueNDRangeKernel(queue, kernel, 2, NULL, globalSize, NULL, 0, NULL, &prof_event);
    if(err < 0) {
        perror("Error: clEnqueueNDRangeKernel");
        exit(1);   
    }

    // Read the result buffer back to the host
    float *C = (float *)malloc(sizeof(float) * SIZE * SIZE);
    err = clEnqueueReadBuffer(queue, result_buffer, CL_TRUE, 0, sizeof(float) * SIZE * SIZE, C, 0, NULL, NULL);
    if(err < 0) {
        perror("Error: clEnqueueReadBuffer");
        exit(1);   
    }

    /* Finish processing the queue and get profiling information */
    clFinish(queue);
    clGetEventProfilingInfo(prof_event, CL_PROFILING_COMMAND_START, sizeof(time_start), &time_start, NULL);
    clGetEventProfilingInfo(prof_event, CL_PROFILING_COMMAND_END, sizeof(time_end), &time_end, NULL);
    total_time = time_end - time_start;

    printf("Time taken to do the addition = %llu ns\n", total_time);

    cl_bool correctFlag = CL_TRUE;

    // Output the result
    // printf("Result:\n");
    for (int i = 0; i < SIZE; i++) {
        for (int j = 0; j < SIZE; j++) {
            if (C[i * SIZE + j] != correct[i * SIZE + j]) {
                correctFlag = CL_FALSE;
            }
            // printf("%.2f ", C[i * SIZE + j]);
        }
        // printf("\n");
    }

    if (correctFlag == CL_TRUE) {
        printf("The result is correct\n");
    } else {
        printf("The result is incorrect\n");
    }  

    clReleaseMemObject(mat1_buffer);
    clReleaseMemObject(mat2_buffer);
    clReleaseMemObject(result_buffer);
    clReleaseCommandQueue(queue);
    clReleaseKernel(kernel);
    clReleaseProgram(program);
    clReleaseContext(context);

    free(A);
    free(B);
    free(C);
    free(correct);

    return 0;

}