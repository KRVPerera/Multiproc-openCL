#define _CRT_SECURE_NO_WARNINGS
#define NUM_FILES 2
#define PROGRAM_FILE_1 "good.cl"
#define PROGRAM_FILE_2 "bad.cl"

#include <stdio.h>
#include <stdlib.h>
#include <opencl_include.h>

int main(int argc, char* const argv[]) {

    cl_platform_id platform;
    cl_device_id device;
    cl_context context;
    cl_int err, i;

    cl_program program;
    FILE *program_handle;
    char *program_buffer[NUM_FILES];
    char *program_log;
    const char *file_name[] = {PROGRAM_FILE_1, PROGRAM_FILE_2};
    const char options[] = "-cl-finite-math-only -cl-no-signed-zeros";  
    size_t program_size[NUM_FILES];
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

    for(i=0; i<NUM_FILES; i++) {

        program_handle = fopen(file_name[i], "r");
        if(program_handle == NULL) {
            perror("Couldn't find the program file");
            exit(1);   
        }
        fseek(program_handle, 0, SEEK_END);
        program_size[i] = ftell(program_handle);
        rewind(program_handle);
        program_buffer[i] = (char*)malloc(program_size[i]+1);
        program_buffer[i][program_size[i]] = '\0';
        fread(program_buffer[i], sizeof(char), program_size[i], 
                program_handle);
        fclose(program_handle);
    }

    program = clCreateProgramWithSource(context, NUM_FILES, (const char**)program_buffer, program_size, &err);
    if(err < 0) {
        perror("Error: clCreateProgramWithSource");
        exit(1);   
    }

    err = clBuildProgram(program, 1, &device, options, NULL, NULL);
    if (err < 0) {
        clGetProgramBuildInfo(program, device, CL_PROGRAM_BUILD_LOG, 0, NULL, &log_size);
        program_log = (char*) malloc(log_size + 1);
        program_log[log_size] = '\0';
        clGetProgramBuildInfo(program, device, CL_PROGRAM_BUILD_LOG, log_size + 1, program_log, NULL);
        printf("%s\n", program_log);
        free(program_log);
        exit(1);
    }

    for(i=0; i<NUM_FILES; i++) {
        free(program_buffer[i]);
    }
    
    clReleaseProgram(program);
    clReleaseContext(context);
    return 0;

}