#include <stdio.h>
#include <stdlib.h>
#include <OpenCL/opencl.h>

int main(int argc, char* const argv[]) {

    cl_platform_id platform;
    cl_device_id device;
    cl_int err;

    cl_context context;

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

    float main_data[100];
    cl_mem main_buffer, sub_buffer;

    void *main_buffer_mem = NULL, *sub_buffer_mem = NULL;
    size_t main_buffer_size, sub_buffer_size;
    cl_buffer_region region;

    main_buffer = clCreateBuffer(context, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, sizeof(main_data), main_data, &err);
    if (err < 0) {
        perror("Error: clCreateBuffer");
        exit(1);
    }
    
    region.origin = 0x100;
    region.size = 20 * sizeof(float);

    // sub_buffer = clCreateSubBuffer(main_buffer, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, CL_BUFFER_CREATE_TYPE_REGION, &region, &err);
    // if (err < 0) {
    //     perror("Error: clCreateSubBuffer");
    //     exit(1);
    // }

    clGetMemObjectInfo(main_buffer, CL_MEM_SIZE, sizeof(main_buffer_size), &main_buffer_size, NULL);
    // clGetMemObjectInfo(sub_buffer, CL_MEM_SIZE, sizeof(sub_buffer_size), &sub_buffer_size, NULL);

    printf("Main buffer size: %lu\n", main_buffer_size);
    // printf("Sub buffer size: %lu\n", sub_buffer_size);

    clGetMemObjectInfo(main_buffer, CL_MEM_HOST_PTR, sizeof(main_buffer_mem), &main_buffer_mem, NULL);
    // clGetMemObjectInfo(sub_buffer, CL_MEM_HOST_PTR, sizeof(sub_buffer_mem), &sub_buffer_mem, NULL);

    printf("Main buffer memory address: %p\n", main_buffer_mem);
    // printf("Sub buffer memory address: %p\n", sub_buffer_mem);

    printf("Main array address: %p\n", main_data);

    clReleaseMemObject(main_buffer);
    clReleaseMemObject(sub_buffer);

    clReleaseContext(context);
    return 0;

}