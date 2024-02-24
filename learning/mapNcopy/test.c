#define _CRT_SECURE_NO_WARNINGS
#define PROGRAM_FILE "blank.cl"
#define KERNEL_NAME "blank"

#include <stdio.h>
#include <stdlib.h>
#include <opencl_include.h>

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

    float data_one[100], data_two[100], result_array[100];
    cl_mem buffer_one, buffer_two;
    void *mapped_memory;

    buffer_one = clCreateBuffer(context, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, sizeof(data_one), data_one, &err);
    if (err < 0) {
        perror("Error: clCreateBuffer");
        exit(1);
    }

    buffer_two = clCreateBuffer(context, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, sizeof(data_two), data_two, &err);

    err = clSetKernelArg(kernel, 0, sizeof(cl_mem), &buffer_one);
    err |= clSetKernelArg(kernel, 1, sizeof(cl_mem), &buffer_two);
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

    err = clEnqueueCopyBuffer(queue, buffer_one, buffer_two, 0, 0, sizeof(data_one), 0, NULL, NULL);
    if(err < 0) {
        perror("Error: clEnqueueCopyBuffer");
        exit(1);   
    }

    mapped_memory = clEnqueueMapBuffer(queue, buffer_two, CL_TRUE, CL_MAP_READ, 0, sizeof(data_two), 0, NULL, NULL, &err);
    if(err < 0) {
        perror("Error: clEnqueueMapBuffer");
        exit(1);   
    }

    memcpy(result_array, mapped_memory, sizeof(data_two));
    err = clEnqueueUnmapMemObject(queue, buffer_two, mapped_memory, 0, NULL, NULL);
    if(err < 0) {
        perror("Error: clEnqueueUnmapMemObject");
        exit(1);   
    }

    clReleaseMemObject(buffer_one);
    clReleaseMemObject(buffer_two);

    clReleaseKernel(kernel);
    clReleaseCommandQueue(queue);
    clReleaseProgram(program);
    clReleaseContext(context);
    return 0;

}