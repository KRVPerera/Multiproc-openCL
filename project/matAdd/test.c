#define _CRT_SECURE_NO_WARNINGS
#define PROGRAM_FILE "matrix_add.cl"
#define KERNEL_NAME "matrix_add"

#include <stdio.h>
#include <stdlib.h>
#include <OpenCL/opencl.h>

int main(int argc, char* const argv[]) {

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

    const uint size = 3;

    int mat1[size][size] = {{1, 2, 3}, {4, 5, 6}, {7, 8, 9}};
    int mat2[size][size] = {{9, 8, 7}, {6, 5, 4}, {3, 2, 1}};
    int result[size][size];

    // Flatten matrices
    int flatMat1[size * size], flatMat2[size * size], flatResult[size * size];
    for (int i = 0; i < size; i++) {
        for (int j = 0; j < size; j++) {
            flatMat1[i * size + j] = mat1[i][j];
            flatMat2[i * size + j] = mat2[i][j];
        }
    }

    cl_mem mat1_buffer, mat2_buffer, result_buffer;

    mat1_buffer = clCreateBuffer(context, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, sizeof(int) * size * size, flatMat1, &err);
    if (err < 0) {
        perror("Error: clCreateBuffer");
        exit(1);
    }

    mat2_buffer = clCreateBuffer(context, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, sizeof(int) * size * size, flatMat2, &err);
    result_buffer = clCreateBuffer(context, CL_MEM_WRITE_ONLY, sizeof(int) * size * size, NULL, &err);

    err = clSetKernelArg(kernel, 0, sizeof(cl_mem), &mat1_buffer);
    if(err < 0) {
        perror("Error: clSetKernelArg");
        exit(1);   
    }

    clSetKernelArg(kernel, 1, sizeof(cl_mem), &mat2_buffer);
    clSetKernelArg(kernel, 2, sizeof(cl_mem), &result_buffer);

    queue = clCreateCommandQueue(context, device, 0, &err);
    if(err < 0) {
        perror("Error: clCreateCommandQueue");
        exit(1);   
    }

    size_t globalSize[2] = {size, size};
    err = clEnqueueNDRangeKernel(queue, kernel, 2, NULL, globalSize, NULL, 0, NULL, NULL);
    if(err < 0) {
        perror("Error: clEnqueueNDRangeKernel");
        exit(1);   
    }

    err = clEnqueueReadBuffer(queue, result_buffer, CL_TRUE, 0, sizeof(int) * size * size, &flatResult, 0, NULL, NULL);
    if(err < 0) {
        perror("Error: clEnqueueReadBuffer");
        exit(1);   
    }

    // Unflatten the result matrix
    for (int i = 0; i < size; i++) {
        for (int j = 0; j < size; j++) {
            result[i][j] = flatResult[i * size + j];
        }
    }

    // Display result matrix
    printf("Resultant matrix after addition:\n");
    for (int i = 0; i < size; i++) {
        for (int j = 0; j < size; j++) {
            printf("%d ", result[i][j]);
        }
        printf("\n");
    }

    clReleaseMemObject(mat1_buffer);
    clReleaseMemObject(mat2_buffer);
    clReleaseMemObject(result_buffer);
    clReleaseKernel(kernel);
    clReleaseCommandQueue(queue);
    clReleaseProgram(program);
    clReleaseContext(context);
    return 0;

}