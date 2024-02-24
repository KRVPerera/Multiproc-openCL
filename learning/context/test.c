#include <stdio.h>
#include <stdlib.h>
#include <opencl_include.h>

int main(int argc, char* const argv[]) {

    cl_platform_id platform;
    cl_device_id device;
    cl_int err;

    cl_context context;
    cl_uint ref_count;

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

    err = clGetContextInfo(context, CL_CONTEXT_REFERENCE_COUNT, sizeof(ref_count), &ref_count, NULL);
    if (err < 0) {
        perror("Error: clGetContextInfo CL_CONTEXT_REFERENCE_COUNT");
        exit(1);
    }

    printf("Initial reference count: %u\n", ref_count);

    clRetainContext(context);
    clGetContextInfo(context, CL_CONTEXT_REFERENCE_COUNT, sizeof(ref_count), &ref_count, NULL);

    printf("Reference count: %u\n", ref_count);

    clReleaseContext(context);
    clGetContextInfo(context, CL_CONTEXT_REFERENCE_COUNT, sizeof(ref_count), &ref_count, NULL);

    printf("Reference count: %u\n", ref_count);

    clReleaseContext(context);
    return 0;

}