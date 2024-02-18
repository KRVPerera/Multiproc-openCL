#include <stdio.h>
#include <stdlib.h>
#include <OpenCL/opencl.h>

int main(int argc, char* const argv[]) {

    cl_platform_id *platforms;
    cl_uint num_platforms, err, j;

    err = clGetPlatformIDs(1, NULL, &num_platforms);
    if (err < 0) {
        perror("Error: clGetPlatformIDs");
        exit(1);
    }

    platforms = (cl_platform_id*) malloc(sizeof(cl_platform_id) * num_platforms);

    clGetPlatformIDs(num_platforms, platforms, NULL);

    for (j = 0; j < num_platforms; j++) {

        char p_name[40];
        char p_vendor[40];
        err = clGetPlatformInfo(platforms[j], CL_PLATFORM_NAME, 40, p_name, NULL);
        if (err < 0) {
            perror("Error: clGetPlatformInfo CL_PLATFORM_NAME");
            exit(1);
        }
        err = clGetPlatformInfo(platforms[j], CL_PLATFORM_VENDOR, 40, p_vendor, NULL);
        if (err < 0) {
            perror("Error: clGetPlatformInfo CL_PLATFORM_VENDOR");
            exit(1);
        }

        cl_uint num_devices, i;

        err = clGetDeviceIDs(platforms[j], CL_DEVICE_TYPE_ALL, 0, NULL, &num_devices);
        if (err < 0) {
            perror("Error: clGetDeviceIDs");
            exit(1);
        }

        cl_device_id* devices = calloc(sizeof(cl_device_id), num_devices);
        clGetDeviceIDs(platforms[j], CL_DEVICE_TYPE_ALL, num_devices, devices, NULL);

        char buf[128];
        cl_uint work_item_dim, compute_units, char_vector_width;

        if (num_devices == 0) {
            printf("no device found!");
        }

        for (i = 0; i < num_devices; i++) {
            clGetDeviceInfo(devices[i], CL_DEVICE_NAME, 128, buf, NULL);
            fprintf(stdout, "Platform: %s, Vendor: %s, Device %s supports ", p_name, p_vendor, buf);

            clGetDeviceInfo(devices[i], CL_DEVICE_VERSION, 128, buf, NULL);
            fprintf(stdout, "%s\n", buf);

            clGetDeviceInfo(devices[i], CL_DEVICE_MAX_WORK_ITEM_DIMENSIONS, sizeof(cl_uint), &work_item_dim, NULL);
            fprintf(stdout, "CL_DEVICE_MAX_WORK_ITEM_DIMENSIONS: %u\n", work_item_dim);

            clGetDeviceInfo(devices[i], CL_DEVICE_MAX_COMPUTE_UNITS, sizeof(cl_uint), &compute_units, NULL);
            fprintf(stdout, "CL_DEVICE_MAX_COMPUTE_UNITS: %u\n", compute_units);

            clGetDeviceInfo(devices[i], CL_DEVICE_PREFERRED_VECTOR_WIDTH_CHAR, sizeof(char_vector_width), &char_vector_width, NULL);
            fprintf(stdout, "CL_DEVICE_PREFERRED_VECTOR_WIDTH_CHAR: %u\n", char_vector_width);
        }

        free(devices);
    }
    free(platforms);

    return 0;
}