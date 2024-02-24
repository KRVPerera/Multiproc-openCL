#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include <stdlib.h>

#include <opencl_include.h>

void checkOpenCLError(cl_int error, const char* message) {
    if (error != CL_SUCCESS) {
        fprintf(stderr, "OpenCL error (%d): %s\n", error, message);
        exit(EXIT_FAILURE);
    }
}

void printSupportedImageFormats(cl_context context, cl_mem_object_type imageType) {
    cl_int error;

    // Get the number of supported image formats
    cl_uint numImageFormats;
    error = clGetSupportedImageFormats(context, CL_MEM_READ_WRITE, imageType, 0, NULL, &numImageFormats);
    checkOpenCLError(error, "clGetSupportedImageFormats");

    // Allocate memory to store the supported image formats
    cl_image_format* supportedFormats = (cl_image_format*)malloc(sizeof(cl_image_format) * numImageFormats);

    // Retrieve the supported image formats
    error = clGetSupportedImageFormats(context, CL_MEM_READ_WRITE, imageType, numImageFormats, supportedFormats, NULL);
    checkOpenCLError(error, "clGetSupportedImageFormats");

    // Print the supported image formats
    printf("Supported Image Formats for image type %d:\n", imageType);
    for (cl_uint i = 0; i < numImageFormats; ++i) {
        printf("Channel Order: %x, Channel Data Type: %x\n", supportedFormats[i].image_channel_order, supportedFormats[i].image_channel_data_type);
    }

    // Cleanup
    free(supportedFormats);
}

int main() {
    cl_int error;

    // Get the OpenCL platform
    cl_platform_id platform;
    error = clGetPlatformIDs(1, &platform, NULL);
    checkOpenCLError(error, "clGetPlatformIDs");

    // Get the first OpenCL device on the platform
    cl_device_id device;
    error = clGetDeviceIDs(platform, CL_DEVICE_TYPE_GPU, 1, &device, NULL);
    checkOpenCLError(error, "clGetDeviceIDs");

    // Create an OpenCL context
    cl_context context = clCreateContext(NULL, 1, &device, NULL, NULL, &error);
    checkOpenCLError(error, "clCreateContext");

    // Print supported image formats for 2D images
    printSupportedImageFormats(context, CL_MEM_OBJECT_IMAGE2D);

    // Release resources
    clReleaseContext(context);

    return 0;
}
