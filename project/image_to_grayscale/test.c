#define KERNEL_RESIZE_IMAGE "resize_image"
#define KERNEL_COLOR_TO_GRAY "color_to_gray"
#define KERNEL_GAUSSIAN_BLUR "gaussian_blur"

#include <pngloader.h>

#define INPUT_FILE "im0.png"
#define OUTPUT_1_FILE "output_1.png"
#define OUTPUT_2_FILE "output_2.png"
#define OUTPUT_3_FILE "output_3.png"

#include <opencl_include.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "config_im_to_g.h"

cl_device_id create_device() {

   cl_platform_id platform;
   cl_device_id device;
   int err;

   err = clGetPlatformIDs(1, &platform, NULL);
   if(err < 0) {
      perror("Couldn't identify a platform");
      exit(1);
   }

   err = clGetDeviceIDs(platform, CL_DEVICE_TYPE_GPU, 1, &device, NULL);
   if(err == CL_DEVICE_NOT_FOUND) {
      err = clGetDeviceIDs(platform, CL_DEVICE_TYPE_CPU, 1, &device, NULL);
   }
   if(err < 0) {
      perror("Couldn't access any devices");
      exit(1);
   }

   return device;
}

cl_program build_program(cl_context ctx, cl_device_id device, const char* filename) {

   cl_program program;
   FILE *program_handle;
   char *program_buffer, *program_log;
   size_t program_size, log_size;
   int err;

   /* Read program file and place content into buffer */
   program_handle = fopen(filename, "r");
   if(program_handle == NULL) {
      perror("Couldn't find the program file");
      exit(1);
   }
   fseek(program_handle, 0, SEEK_END);
   program_size = ftell(program_handle);
   rewind(program_handle);
   program_buffer = (char*)malloc(program_size + 1);
   program_buffer[program_size] = '\0';
   fread(program_buffer, sizeof(char), program_size, program_handle);
   fclose(program_handle);

   program = clCreateProgramWithSource(ctx, 1,
      (const char**)&program_buffer, &program_size, &err);
   if(err < 0) {
      perror("Couldn't create the program");
      exit(1);
   }
   free(program_buffer);

   err = clBuildProgram(program, 0, NULL, NULL, NULL, NULL);
   if(err < 0) {

      clGetProgramBuildInfo(program, device, CL_PROGRAM_BUILD_LOG,
            0, NULL, &log_size);
      program_log = (char*) malloc(log_size + 1);
      program_log[log_size] = '\0';
      clGetProgramBuildInfo(program, device, CL_PROGRAM_BUILD_LOG,
            log_size + 1, program_log, NULL);
      printf("%s\n", program_log);
      free(program_log);
      exit(1);
   }

   return program;
}

// cl_ulong getExecutionTime(cl_event event) {
//     cl_ulong start_time, end_time;
//     clGetEventProfilingInfo(event, CL_PROFILING_COMMAND_START, sizeof(start_time), &start_time, NULL);
//     clGetEventProfilingInfo(event, CL_PROFILING_COMMAND_END, sizeof(end_time), &end_time, NULL);
//     return end_time - start_time;
// }

void resize_image(cl_context context, cl_kernel kernel, cl_command_queue queue, const Image *im0, Image *output_im0, cl_event *read_event, cl_event *downsample_event) {

    /* Image data */
    cl_mem input_image, output_image;
    cl_image_format input_format, output_format;
    int err;

    cl_ulong read_time, time_to_downsample;

    size_t width = im0 -> width;
    size_t height = im0 -> height;
    size_t new_width = width / 4;
    size_t new_height = height / 4;

    input_format.image_channel_order = CL_RGBA;
    input_format.image_channel_data_type = CL_UNORM_INT8;

    output_format.image_channel_order = CL_RGBA;
    output_format.image_channel_data_type = CL_UNORM_INT8;

    /* Create input image object */
    input_image = clCreateImage2D(context, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, &input_format, width, height, 0, (void*)im0 -> image, &err);
    if(err < 0) {
        printf("resize_image: Couldn't create the input image object");
        exit(1);
    };

    /* Create output image object */
    output_image = clCreateImage2D(context, CL_MEM_WRITE_ONLY, &output_format, new_width, new_height, 0, NULL, &err);
    if(err < 0) {
        perror("resize_image: Couldn't create the input image object");
        exit(1);
    };

    // Set kernel arguments
    err = clSetKernelArg(kernel, 0, sizeof(cl_mem), &input_image);
    if(err < 0) {
        perror("resize_image, Error: clSetKernelArg, inputImage");
        exit(1);
    }

    err = clSetKernelArg(kernel, 1, sizeof(cl_mem), &output_image);
    if(err < 0) {
        perror("resize_image, Error: clSetKernelArg, outputImage");
        exit(1);
    }

    // Execute the OpenCL kernel
    size_t globalWorkSize[2] = { new_width, new_height };
    err = clEnqueueNDRangeKernel(queue, kernel, 2, NULL, globalWorkSize, NULL, 0, NULL, downsample_event);
    if(err < 0) {
        perror("resize_image, Error: clEnqueueNDRangeKernel");
        exit(1);
    }

    // Read the output image back to the host
    err = clEnqueueReadImage(queue, output_image, CL_TRUE, (size_t[3]){0, 0, 0}, (size_t[3]){new_width, new_height, 1},
                             0, 0, (void*)output_im0 -> image, 0, NULL, read_event);
    if(err < 0) {
        perror("resize_image, Error: clEnqueueReadImage");
        exit(1);
    }

    clFinish(queue);

    output_im0 -> width = new_width;
    output_im0 -> height = new_height;

    cl_ulong start_time, end_time;
    cl_ulong start_time_read, end_time_read;
    clGetEventProfilingInfo(*downsample_event, CL_PROFILING_COMMAND_START, sizeof(start_time), &start_time, NULL);
    clGetEventProfilingInfo(*downsample_event, CL_PROFILING_COMMAND_END, sizeof(end_time), &end_time, NULL);
    time_to_downsample = end_time - start_time;

    clGetEventProfilingInfo(*read_event, CL_PROFILING_COMMAND_START, sizeof(start_time_read), &start_time_read, NULL);
    clGetEventProfilingInfo(*read_event, CL_PROFILING_COMMAND_END, sizeof(end_time_read), &end_time_read, NULL);
    read_time = end_time_read - start_time_read;

    printf("Time taken to do the downsampling = %llu ns\n", time_to_downsample);
    printf("Time taken to read the output image (downsampling) = %llu ns\n", read_time);
}

void convert_image_to_gray(cl_context context, cl_kernel kernel, cl_command_queue queue, const Image *im0, Image *output_im0, cl_event *read_event, cl_event *grayscale_event) {

    /* Image data */
    cl_mem input_image, output_image;
    cl_image_format input_format, output_format;
    int err;

    cl_ulong read_time, time_to_grayscale;

    size_t width = im0 -> width;
    size_t height = im0 -> height;

    input_format.image_channel_order = CL_RGBA;
    input_format.image_channel_data_type = CL_UNORM_INT8;

    output_format.image_channel_order = CL_RGBA;
    output_format.image_channel_data_type = CL_UNORM_INT8;

    /* Create input image object */
    input_image = clCreateImage2D(context, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, &input_format, width, height, 0, (void*)im0 -> image, &err);
    if(err < 0) {
        printf("convert_image_to_gray: Couldn't create the input image object");
        exit(1);
    };

    /* Create output image object */
    output_image = clCreateImage2D(context, CL_MEM_WRITE_ONLY, &output_format, width, height, 0, NULL, &err);
    if(err < 0) {
        perror("convert_image_to_gray: Couldn't create the input image object");
        exit(1);
    };

    // Set kernel arguments
    err = clSetKernelArg(kernel, 0, sizeof(cl_mem), &input_image);
    if(err < 0) {
        perror("convert_image_to_gray, Error: clSetKernelArg, inputImage");
        exit(1);
    }

    err = clSetKernelArg(kernel, 1, sizeof(cl_mem), &output_image);
    if(err < 0) {
        perror("convert_image_to_gray, Error: clSetKernelArg, outputImage");
        exit(1);
    }

    // Execute the OpenCL kernel
    size_t globalWorkSize[2] = { width, height };
    err = clEnqueueNDRangeKernel(queue, kernel, 2, NULL, globalWorkSize, NULL, 0, NULL, grayscale_event);
    if(err < 0) {
        perror("convert_image_to_gray, Error: clEnqueueNDRangeKernel");
        exit(1);
    }

    // Read the output image back to the host
    err = clEnqueueReadImage(queue, output_image, CL_TRUE, (size_t[3]){0, 0, 0}, (size_t[3]){width, height, 1},
                             0, 0, (void*)output_im0 -> image, 0, NULL, read_event);
    if(err < 0) {
        perror("convert_image_to_gray, Error: clEnqueueReadImage");
        exit(1);
    }

    clFinish(queue);

    output_im0 -> width = width;
    output_im0 -> height = height;

    cl_ulong start_time, end_time;
    cl_ulong start_time_read, end_time_read;
    clGetEventProfilingInfo(*grayscale_event, CL_PROFILING_COMMAND_START, sizeof(start_time), &start_time, NULL);
    clGetEventProfilingInfo(*grayscale_event, CL_PROFILING_COMMAND_END, sizeof(end_time), &end_time, NULL);
    time_to_grayscale = end_time - start_time;

    clGetEventProfilingInfo(*read_event, CL_PROFILING_COMMAND_START, sizeof(start_time_read), &start_time_read, NULL);
    clGetEventProfilingInfo(*read_event, CL_PROFILING_COMMAND_END, sizeof(end_time_read), &end_time_read, NULL);
    read_time = end_time_read - start_time_read;

    printf("Time taken to do the gray scaling = %llu ns\n", time_to_grayscale);
    printf("Time taken to read the output image (gray scaling) = %llu ns\n", read_time);
}

void apply_gaussian_blur(cl_context context, cl_kernel kernel, cl_command_queue queue, const Image *im0, Image *output_im0, cl_event *read_event, cl_event *gaussian_event) {

    /* Image data */
    cl_mem input_image, output_image;
    cl_image_format input_format, output_format;
    int err;

    cl_ulong read_time, time_to_gaussian_blur;

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
    err = clEnqueueNDRangeKernel(queue, kernel, 2, NULL, globalWorkSize, NULL, 0, NULL, gaussian_event);
    if(err < 0) {
        perror("apply_gaussian_blur, Error: clEnqueueNDRangeKernel");
        exit(1);
    }

    // Read the output image back to the host
    err = clEnqueueReadImage(queue, output_image, CL_TRUE, (size_t[3]){0, 0, 0}, (size_t[3]){width, height, 1},
                             0, 0, (void*)output_im0 -> image, 0, NULL, read_event);
    if(err < 0) {
        perror("apply_gaussian_blur, Error: clEnqueueReadImage");
        exit(1);
    }

    clFinish(queue);

    output_im0 -> width = width;
    output_im0 -> height = height;

    cl_ulong start_time, end_time;
    cl_ulong start_time_read, end_time_read;
    clGetEventProfilingInfo(*gaussian_event, CL_PROFILING_COMMAND_START, sizeof(start_time), &start_time, NULL);
    clGetEventProfilingInfo(*gaussian_event, CL_PROFILING_COMMAND_END, sizeof(end_time), &end_time, NULL);
    time_to_gaussian_blur = end_time - start_time;

    clGetEventProfilingInfo(*read_event, CL_PROFILING_COMMAND_START, sizeof(start_time_read), &start_time_read, NULL);
    clGetEventProfilingInfo(*read_event, CL_PROFILING_COMMAND_END, sizeof(end_time_read), &end_time_read, NULL);
    read_time = end_time_read - start_time_read;

    printf("Time taken to do the gaussian blur = %llu ns\n", time_to_gaussian_blur);
    printf("Time taken to read the output image (gaussian blur) = %llu ns\n", read_time);
}

int main() {

    cl_device_id device;
    cl_context context;
    cl_command_queue queue;
    cl_program program;
    cl_int err;

    cl_kernel *kernels, kernel_resize_image, kernel_color_to_gray, kernel_gaussian_blur;
    char kernel_name[20];
    cl_uint i, num_kernels;

    /* Profiling data */
    cl_event grayscale_read_event, grayscale_event, resize_read_event, resize_event, gaussian_read_event, gaussian_event;

    size_t width, height, new_width, new_height;

    /* Open input file and read image data */
    Image *im0 = readImage(INPUT_FILE);
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

    queue = clCreateCommandQueue(context, device, 0, &err);
    if(err < 0) {
        perror("Error: clCreateCommandQueue");
        exit(1);
    }

    /* Resize image size */
    resize_image(context, kernel_resize_image, queue, im0, output_1_im0, &resize_read_event, &resize_event);

    /* Convert color image to gray scale image */
    convert_image_to_gray(context, kernel_color_to_gray, queue, output_1_im0, output_2_im0, &grayscale_read_event, &grayscale_event);

    /* Apply gaussian blur with 5 x 5 kernel */
    apply_gaussian_blur(context, kernel_gaussian_blur, queue, output_2_im0, output_3_im0, &gaussian_read_event, &gaussian_event);

    saveImage(OUTPUT_1_FILE, output_1_im0);
    saveImage(OUTPUT_2_FILE, output_2_im0);
    saveImage(OUTPUT_3_FILE, output_3_im0);

    /* Deallocate resources */
    freeImage(im0);
    freeImage(output_1_im0);
    freeImage(output_2_im0);
    freeImage(output_3_im0);
    clReleaseEvent(resize_read_event);
    clReleaseEvent(resize_event);
    clReleaseEvent(grayscale_read_event);
    clReleaseEvent(grayscale_event);
    clReleaseEvent(gaussian_read_event);
    clReleaseEvent(gaussian_event);
    clReleaseKernel(kernel_color_to_gray);
    clReleaseKernel(kernel_resize_image);
    clReleaseKernel(kernel_gaussian_blur);
    clReleaseCommandQueue(queue);
    clReleaseProgram(program);
    clReleaseContext(context);
    return 0;
}