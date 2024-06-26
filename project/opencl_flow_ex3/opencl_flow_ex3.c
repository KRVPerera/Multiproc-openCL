#define KERNEL_RESIZE_IMAGE "resize_image"
#define KERNEL_COLOR_TO_GRAY "color_to_gray"
#define KERNEL_GAUSSIAN_BLUR "gaussian_blur"

#include <pngloader.h>
#include <config.h>

#include <opencl_flow_ex3.h>
#include <opencl_flow_ex5.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "config_im_to_g.h"

void apply_gaussian_blur(cl_context context, cl_kernel kernel, cl_command_queue queue, const Image *im0, Image *output_im0, BENCHMARK_MODE benchmark) {

    /* Image data */
    cl_mem input_image, output_image;
    cl_image_format input_format, output_format;
    int err;

    cl_ulong read_time, time_to_gaussian_blur;
    
    cl_event gaussian_read_event, gaussian_event;

    size_t width = im0 -> width;
    size_t height = im0 -> height;

    input_format.image_channel_order = CL_RGBA;
    input_format.image_channel_data_type = CL_UNORM_INT8;

    output_format.image_channel_order = CL_RGBA;
    output_format.image_channel_data_type = CL_UNORM_INT8;

    /* Create input image object */
    input_image = clCreateImage2D(context, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, &input_format, width, height, 0, (void*)im0 -> image, &err);
    if(err < 0) {
        printf("apply_gaussian_blur: Couldn't create the input image object");
        exit(1);
    };

    /* Create output image object */
    output_image = clCreateImage2D(context, CL_MEM_WRITE_ONLY, &output_format, width, height, 0, NULL, &err);
    if(err < 0) {
        perror("apply_gaussian_blur: Couldn't create the input image object");
        exit(1);
    };

    // Set kernel arguments
    err = clSetKernelArg(kernel, 0, sizeof(cl_mem), &input_image);
    if(err < 0) {
        perror("apply_gaussian_blur, Error: clSetKernelArg, inputImage");
        exit(1);
    }

    err = clSetKernelArg(kernel, 1, sizeof(cl_mem), &output_image);
    if(err < 0) {
        perror("apply_gaussian_blur, Error: clSetKernelArg, outputImage");
        exit(1);
    }

    // Execute the OpenCL kernel
    size_t globalWorkSize[2] = { width, height };
    err = clEnqueueNDRangeKernel(queue, kernel, 2, NULL, globalWorkSize, NULL, 0, NULL, &gaussian_event);
    if(err < 0) {
        perror("apply_gaussian_blur, Error: clEnqueueNDRangeKernel");
        exit(1);
    }

    // Read the output image back to the host
    err = clEnqueueReadImage(queue, output_image, CL_TRUE, (size_t[3]){0, 0, 0}, (size_t[3]){width, height, 1},
                             0, 0, (void*)output_im0 -> image, 0, NULL, &gaussian_read_event);
    if(err < 0) {
        perror("apply_gaussian_blur, Error: clEnqueueReadImage");
        exit(1);
    }

    clFinish(queue);

    output_im0 -> width = width;
    output_im0 -> height = height;

    time_to_gaussian_blur = getExecutionTime(gaussian_event);
    read_time = getExecutionTime(gaussian_read_event);

    clReleaseEvent(gaussian_read_event);
    clReleaseEvent(gaussian_event);

    if (benchmark == DO_NOT_BENCHMARK) {
        printf("Time taken to do the gaussian blur = %llu ns\n", time_to_gaussian_blur);
        printf("Time taken to read the output image (gaussian blur) = %llu ns\n", read_time);
    }
}

void openclFlowEx3(BENCHMARK_MODE benchmark) {
    printf("OpenCL Flow Example 3 STARTED\n");
    cl_device_id device;
    cl_context context;
    cl_command_queue queue;
    cl_program program;
    cl_int err;

    cl_kernel *kernels, kernel_resize_image, kernel_color_to_gray, kernel_gaussian_blur;
    char kernel_name[20];
    cl_uint i, num_kernels;

    size_t width, height, new_width, new_height;

    /* Open input file and read image data */
    Image *im0 = readImage(INPUT_FILE_0);
    width = im0 -> width;
    height = im0 -> height;
    new_width = width / 4;
    new_height = height / 4;

    Image *output_1_im0 = createEmptyImage(new_width, new_height);
    Image *output_2_im0 = createEmptyImage(new_width, new_height);
    Image *output_3_im0 = createEmptyImage(new_width, new_height);

    device = create_device();

    context = clCreateContext(NULL, 1, &device, NULL, NULL, &err);
    if (err < 0) {
        perror("Error: clCreateContext");
        exit(1);
    }

    program = build_program(context, device, PROGRAM_FILE);

    /* Find out how many kernels are in the source file */
    err = clCreateKernelsInProgram(program, 0, NULL, &num_kernels);
    if(err < 0) {
        perror("Couldn't find any kernels");
        exit(1);
    }

    printf("Number of kernels: %u\n", num_kernels);

    /* Create a kernel for each function */
    kernels = (cl_kernel*) malloc(num_kernels * sizeof(cl_kernel));
    clCreateKernelsInProgram(program, num_kernels, kernels, NULL);

    // /* Search for the named kernel */
    for(i=0; i<num_kernels; i++) {
        clGetKernelInfo(kernels[i], CL_KERNEL_FUNCTION_NAME,
                        sizeof(kernel_name), kernel_name, NULL);
        if(strcmp(kernel_name, KERNEL_RESIZE_IMAGE) == 0) {
            kernel_resize_image = kernels[i];
            printf("Found resize_image kernel at index %u.\n", i);
        } else if(strcmp(kernel_name, KERNEL_COLOR_TO_GRAY) == 0) {
            kernel_color_to_gray = kernels[i];
            printf("Found color_to_gray kernel at index %u.\n", i);
        } else if(strcmp(kernel_name, KERNEL_GAUSSIAN_BLUR) == 0) {
            kernel_gaussian_blur = kernels[i];
            printf("Found gaussian_blur kernel at index %u.\n", i);
        }
    }
    // cl_command_queue_properties props[3] = {CL_QUEUE_PROPERTIES, CL_QUEUE_PROFILING_ENABLE, 0};
    // queue = clCreateCommandQueueWithProperties(context, device, props, &err);
    queue = clCreateCommandQueue(context, device, CL_QUEUE_PROFILING_ENABLE, &err);
    if(err < 0) {
        perror("Error: clCreateCommandQueue");
        exit(1);
    }

    /* Resize image size */
    resize_image(context, kernel_resize_image, queue, im0, output_1_im0, benchmark);

    /* Convert color image to gray scale image */
    convert_image_to_gray(context, kernel_color_to_gray, queue, output_1_im0, output_2_im0, benchmark);

    /* Apply gaussian blur with 5 x 5 kernel */
    apply_gaussian_blur(context, kernel_gaussian_blur, queue, output_2_im0, output_3_im0, benchmark);

    saveImage(OUTPUT_1_RESIZE_OPENCL_FILE, output_1_im0);
    saveImage(OUTPUT_1_BW_OPENCL_FILE, output_2_im0);
    saveImage(OUTPUT_1_BLUR_OPENCL_FILE, output_3_im0);

    /* Deallocate resources */
    freeImage(im0);
    freeImage(output_1_im0);
    freeImage(output_2_im0);
    freeImage(output_3_im0);
    clReleaseKernel(kernel_color_to_gray);
    clReleaseKernel(kernel_resize_image);
    clReleaseKernel(kernel_gaussian_blur);
    clReleaseCommandQueue(queue);
    clReleaseProgram(program);
    clReleaseContext(context);

    printf("OpenCL Flow Example 3 ENDED\n");
}
