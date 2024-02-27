#define PROGRAM_FILE "color_to_gray.cl"
#define KERNEL_NAME "color_to_gray"

#include <pngloader.h>

#define OUTPUT_FILE "output.png"

#include <stdio.h>
#include <stdlib.h>
#include <opencl_include.h>

int image_to_grayscale(char* image_path) {
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

    /* Image data */
    cl_mem input_image, output_image;
    cl_image_format input_format, output_format;

    cl_event prof_event, read_event;
    cl_ulong time_queued, time_start, time_end, total_time, read_start, read_end, read_total_time;

    size_t width, height;

    /* Open input file and read image data */
    Image *im0 = loadImage(image_path);
    Image *output_im0 = createNewImage(im0->width, im0->height);
    width = im0 -> width;
    height = im0 -> height;
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

    input_format.image_channel_order = CL_RGBA;
    input_format.image_channel_data_type = CL_UNORM_INT8;

    output_format.image_channel_order = CL_RGBA;
    output_format.image_channel_data_type = CL_UNORM_INT8;

    /* Create input image object */
    input_image = clCreateImage2D(context, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, &input_format, width, height, 0, (void*)im0 -> image, &err);
    if(err < 0) {
        printf("Couldn't create the input image object");
        exit(1);
    };

    /* Create output image object */
    output_image = clCreateImage2D(context, CL_MEM_WRITE_ONLY, &output_format, width, height, 0, NULL, &err);
    if(err < 0) {
        perror("Couldn't create the input image object");
        exit(1);
    };

    // Set kernel arguments
    err = clSetKernelArg(kernel, 0, sizeof(cl_mem), &input_image);
    if(err < 0) {
        perror("Error: clSetKernelArg, inputImage");
        exit(1);
    }

    err = clSetKernelArg(kernel, 1, sizeof(cl_mem), &output_image);
    if(err < 0) {
        perror("Error: clSetKernelArg, outputImage");
        exit(1);
    }

    queue = clCreateCommandQueue(context, device, 0, &err);
    if(err < 0) {
        perror("Error: clCreateCommandQueue");
        exit(1);
    }

    // Execute the OpenCL kernel
    size_t globalWorkSize[2] = { width, height };
    err = clEnqueueNDRangeKernel(queue, kernel, 2, NULL, globalWorkSize, NULL, 0, NULL, &prof_event);
    if(err < 0) {
        perror("Error: clEnqueueNDRangeKernel");
        exit(1);
    }

    // Read the output image back to the host
    err = clEnqueueReadImage(queue, output_image, CL_TRUE, (size_t[3]){0, 0, 0}, (size_t[3]){width, height, 1},
                             0, 0, (void*)output_im0 -> image, 0, NULL, &read_event);
    if(err < 0) {
        perror("Error: clEnqueueReadImage");
        exit(1);
    }

    /* Finish processing the queue and get profiling information */
    clFinish(queue);
    clGetEventProfilingInfo(prof_event, CL_PROFILING_COMMAND_QUEUED, sizeof(time_queued), &time_queued, NULL);
    clGetEventProfilingInfo(prof_event, CL_PROFILING_COMMAND_START, sizeof(time_start), &time_start, NULL);
    clGetEventProfilingInfo(prof_event, CL_PROFILING_COMMAND_END, sizeof(time_end), &time_end, NULL);
    total_time = time_end - time_queued;

    clGetEventProfilingInfo(read_event, CL_PROFILING_COMMAND_START, sizeof(read_start), &read_start, NULL);
    clGetEventProfilingInfo(read_event, CL_PROFILING_COMMAND_END, sizeof(read_end), &read_end, NULL);
    read_total_time = read_end - read_start;

    printf("Time taken to do the gray scaling = %llu ns\n", total_time);
    printf("Time taken to read the output image = %llu ns\n", read_total_time);

    /* Create output PNG file and write data */
    output_im0 -> width = width;
    output_im0 -> height = height;

    printf("Width output: %d\n", output_im0 -> width);
    printf("Height output: %d\n", output_im0 -> height);
    saveImage(OUTPUT_FILE, output_im0);

    /* Deallocate resources */
    freeImage(im0);
    freeImage(output_im0);
    clReleaseMemObject(input_image);
    clReleaseMemObject(output_image);
    clReleaseKernel(kernel);
    clReleaseCommandQueue(queue);
    clReleaseProgram(program);
    clReleaseContext(context);
    return 0;
}

int main() {
    printf("Hello, World!\n");
    return image_to_grayscale("im0.png");
}