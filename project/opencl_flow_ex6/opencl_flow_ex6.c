#define KERNEL_RESIZE_IMAGE "resize_image"
#define KERNEL_COLOR_TO_GRAY "color_to_gray"
#define KERNEL_ZNCC_LEFT "left_disparity"
#define KERNEL_ZNCC_RIGHT "right_disparity"
#define KERNEL_CROSS_CHECK "crosscheck"
#define KERNEL_OCCLUSION_FILL "occlusion_fill"

#include <config.h>
#include <pngloader.h>

#include "config_im_to_g.h"
#include <opencl_flow_ex5.h>
#include <opencl_flow_ex6.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void printDeviceInformation(cl_device_id device_id);

void apply_occlusion_fill_6(cl_context context, cl_kernel kernel, cl_command_queue queue, const Image *im0, Image *output_im0)
{

    /* Image data */
    cl_mem input_image, output_image;
    cl_image_format input_format, output_format;
    int err;

    cl_ulong read_time, time_to_occlustion_fill;

    cl_event occlustion_fill_read_event, occlustion_fill_event;

    const size_t width = im0->width;
    const size_t height = im0->height;

    input_format.image_channel_order = CL_RGBA;
    input_format.image_channel_data_type = CL_UNORM_INT8;

    output_format.image_channel_order = CL_RGBA;
    output_format.image_channel_data_type = CL_UNORM_INT8;

    /* Create input image object */
    input_image = clCreateImage2D(context, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, &input_format, width, height, 0, (void *)im0->image, &err);
    if (err < 0) {
        printf("occlustion_fill: Couldn't create the input image 0 object");
        exit(1);
    };

    /* Create output image object */
    output_image = clCreateImage2D(context, CL_MEM_READ_WRITE | CL_MEM_ALLOC_HOST_PTR, &output_format, width, height, 0, NULL, &err);
    if (err < 0) {
        perror("occlustion_fill: Couldn't create the input image object");
        exit(1);
    };

    // Set kernel arguments
    err = clSetKernelArg(kernel, 0, sizeof(cl_mem), &input_image);
    if (err < 0) {
        perror("occlustion_fill, Error: clSetKernelArg, inputImage");
        exit(1);
    }

    err = clSetKernelArg(kernel, 1, sizeof(cl_mem), &output_image);
    if (err < 0) {
        perror("occlustion_fill, Error: clSetKernelArg, outputImage");
        exit(1);
    }

    // Execute the OpenCL kernel
    size_t globalWorkSize[2] = { width, height };
    err = clEnqueueNDRangeKernel(queue, kernel, 2, NULL, globalWorkSize, NULL, 0, NULL, &occlustion_fill_event);
    if (err < 0) {
        perror("occlustion_fill, Error: clEnqueueNDRangeKernel");
        exit(1);
    }

    // Read the output image back to the host
    err = clEnqueueReadImage(queue,
      output_image,
      CL_TRUE,
      (size_t[3]){ 0, 0, 0 },
      (size_t[3]){ width, height, 1 },
      0,
      0,
      (void *)output_im0->image,
      0,
      NULL,
      &occlustion_fill_read_event);
    if (err < 0) {
        perror("occlustion_fill, Error: clEnqueueReadImage");
        exit(1);
    }

    clFinish(queue);

    output_im0->width = width;
    output_im0->height = height;

    time_to_occlustion_fill = getExecutionTime(occlustion_fill_event);
    read_time = getExecutionTime(occlustion_fill_read_event);

    clReleaseEvent(occlustion_fill_read_event);
    clReleaseEvent(occlustion_fill_event);

    printf("Time taken to do the occlustion_fill = %llu ns\n", time_to_occlustion_fill);
    printf("Time taken to read the output image (occlustion_fill) = %llu ns\n", read_time);
}

void openclFlowEx6(void)
{
    printf("OpenCL Flow 6 STARTED\n");
    cl_device_id device;
    cl_context context;
    cl_command_queue queue;
    cl_program program;
    cl_int err;

    cl_kernel *kernels, kernel_resize_image, kernel_color_to_gray, kernel_zncc_left, kernel_zncc_right, kernel_cross_check, kernel_occlusion_fill;
    char kernel_name[20];
    cl_uint i, num_kernels;

    /* Open input file and read image data */
    Image *im0 = readImage(INPUT_FILE_0);
    Image *im1 = readImage(INPUT_FILE_1);
    const size_t width = im0->width;
    const size_t height = im0->height;
    const size_t new_width = width / 4;
    const size_t new_height = height / 4;

    Image *output_1_resized_im0 = createEmptyImage(new_width, new_height);
    Image *output_1_bw_im0 = createEmptyImage(new_width, new_height);
    Image *output_left_disparity_im0 = createEmptyImage(new_width, new_height);

    Image *output_2_resized_im0 = createEmptyImage(new_width, new_height);
    Image *output_2_bw_im0 = createEmptyImage(new_width, new_height);
    Image *output_right_disparity_im0 = createEmptyImage(new_width, new_height);
    Image *left_crosscheck_im0 = createEmptyImage(new_width, new_height);
    Image *output_left_occlusion_im0 = createEmptyImage(new_width, new_height);

    device = create_device();
    printDeviceInformation(device);

    context = clCreateContext(NULL, 1, &device, NULL, NULL, &err);
    if (err < 0) {
        perror("Error: clCreateContext");
        exit(1);
    }

    program = build_program(context, device, PROGRAM_FILE);

    /* Find out how many kernels are in the source file */
    err = clCreateKernelsInProgram(program, 0, NULL, &num_kernels);
    if (err < 0) {
        perror("Couldn't find any kernels");
        exit(1);
    }

    printf("Number of kernels: %u\n", num_kernels);

    /* Create a kernel for each function */
    kernels = (cl_kernel *)malloc(num_kernels * sizeof(cl_kernel));
    clCreateKernelsInProgram(program, num_kernels, kernels, NULL);

    // /* Search for the named kernel */
    for (i = 0; i < num_kernels; i++) {
        clGetKernelInfo(kernels[i], CL_KERNEL_FUNCTION_NAME, sizeof(kernel_name), kernel_name, NULL);
        if (strcmp(kernel_name, KERNEL_RESIZE_IMAGE) == 0) {
            kernel_resize_image = kernels[i];
            printf("Found resize_image kernel at index %u.\n", i);
        } else if (strcmp(kernel_name, KERNEL_COLOR_TO_GRAY) == 0) {
            kernel_color_to_gray = kernels[i];
            printf("Found color_to_gray kernel at index %u.\n", i);
        } else if (strcmp(kernel_name, KERNEL_ZNCC_LEFT) == 0) {
            kernel_zncc_left = kernels[i];
            printf("Found zncc_left kernel at index %u.\n", i);
        } else if (strcmp(kernel_name, KERNEL_ZNCC_RIGHT) == 0) {
            kernel_zncc_right = kernels[i];
            printf("Found zncc_right kernel at index %u.\n", i);
        } else if (strcmp(kernel_name, KERNEL_CROSS_CHECK) == 0) {
            kernel_cross_check = kernels[i];
            printf("Found cross_check kernel at index %u.\n", i);
        } else if (strcmp(kernel_name, KERNEL_OCCLUSION_FILL) == 0) {
            kernel_occlusion_fill = kernels[i];
            printf("Found occlustion kernel at index %u.\n", i);
        }
    }
    // cl_command_queue_properties props[3] = {CL_QUEUE_PROPERTIES, CL_QUEUE_PROFILING_ENABLE, 0};
    // queue = clCreateCommandQueueWithProperties(context, device, props, &err);
    queue = clCreateCommandQueue(context, device, CL_QUEUE_PROFILING_ENABLE, &err);
    if (err < 0) {
        perror("Error: clCreateCommandQueue");
        exit(1);
    }

    /* Resize image size */
    resize_image(context, kernel_resize_image, queue, im0, output_1_resized_im0);
    /* Convert color image to gray scale image */
    convert_image_to_gray(context, kernel_color_to_gray, queue, output_1_resized_im0, output_1_bw_im0);

    /* Resize image size */
    resize_image(context, kernel_resize_image, queue, im1, output_2_resized_im0);
    /* Convert color image to gray scale image */
    convert_image_to_gray(context, kernel_color_to_gray, queue, output_2_resized_im0, output_2_bw_im0);

    /* Apply zncc kernel */
    apply_zncc(device, context, kernel_zncc_left, queue, output_1_bw_im0, output_2_bw_im0, output_left_disparity_im0);

    /* Apply zncc kernel */
    apply_zncc(device, context, kernel_zncc_right, queue, output_2_bw_im0, output_1_bw_im0, output_right_disparity_im0);

    /* Apply left crosscheck kernel */
    apply_crosscheck(context, kernel_cross_check, queue, output_left_disparity_im0, output_right_disparity_im0, left_crosscheck_im0);

    /* Apply left occlustion fill kernel */
    apply_occlusion_fill_6(context, kernel_occlusion_fill, queue, left_crosscheck_im0, output_left_occlusion_im0);

    saveImage(OUTPUT_1_RESIZE_OPENCL_FILE, output_1_resized_im0);
    saveImage(OUTPUT_1_BW_OPENCL_FILE, output_1_bw_im0);
    saveImage(OUTPUT_1_LEFT_DISPARITY_OPENCL_FILE, output_left_disparity_im0);

    saveImage(OUTPUT_2_RESIZE_OPENCL_FILE, output_2_resized_im0);
    saveImage(OUTPUT_2_BW_OPENCL_FILE, output_2_bw_im0);
    saveImage(OUTPUT_2_LEFT_DISPARITY_OPENCL_FILE, output_right_disparity_im0);

    saveImage(OUTPUT_3_CROSSCHECK_LEFT_OPENCL_FILE, left_crosscheck_im0);

    saveImage(OUTPUT_4_OCCLUSTION_LEFT_OPENCL_FILE, output_left_occlusion_im0);

    /* Deallocate resources */
    freeImage(im0);
    freeImage(im1);
    freeImage(output_1_resized_im0);
    freeImage(output_1_bw_im0);
    freeImage(output_left_disparity_im0);
    freeImage(output_2_resized_im0);
    freeImage(output_2_bw_im0);
    freeImage(output_right_disparity_im0);
    clReleaseKernel(kernel_color_to_gray);
    clReleaseKernel(kernel_resize_image);
    clReleaseKernel(kernel_zncc_left);
    clReleaseKernel(kernel_zncc_right);
    clReleaseKernel(kernel_cross_check);
    clReleaseKernel(kernel_occlusion_fill);
    clReleaseCommandQueue(queue);
    clReleaseProgram(program);
    clReleaseContext(context);

    printf("OpenCL Flow 6 ENDED\n");
}

void printDeviceInformation(cl_device_id device_id)
{
    printf("===== start printing device information =====\n");
    cl_device_fp_config flag;
    cl_platform_id platform_id;
    char buf[128];
    char p_name[40];
    char p_vendor[40];
    size_t time_res, local_size;
    cl_uint work_item_dim, compute_units, char_vector_width, global_mem_size, global_mem_cache, buffer_size, local_mem_size, cache_size, clock_freq;
    clGetDeviceInfo(device_id, CL_DEVICE_PLATFORM, sizeof(cl_platform_id), &platform_id, NULL);
    cl_int err = clGetPlatformInfo(platform_id, CL_PLATFORM_NAME, 40, p_name, NULL);
    if (err < 0) {
        perror("Couldn't read platform name");
    } else {
        printf("Platform name\t: %s\n", p_name);
    }
    err = clGetPlatformInfo(platform_id, CL_PLATFORM_VENDOR, 40, p_vendor, NULL);
    if (err < 0) {
        perror("Couldn't read platform vendor");
    } else {
        printf("Platform vendor\t: %s\n", p_vendor);
    }
    clGetDeviceInfo(device_id, CL_DEVICE_NAME, 128, buf, NULL);
    fprintf(stdout, "Device\t\t: %s\n", buf);
    err = clGetDeviceInfo(device_id, CL_DEVICE_VERSION, 128, buf, NULL);
    if (err < 0) {
        perror("Couldn't read device version");
    } else {
        printf("OpenCL version \t: %s\n", buf);
    }

    clGetDeviceInfo(device_id, CL_DEVICE_MAX_WORK_ITEM_DIMENSIONS, sizeof(cl_uint), &work_item_dim, NULL);
    fprintf(stdout, "Work dimensions \t: %u\n", work_item_dim);

    clGetDeviceInfo(device_id, CL_DEVICE_MAX_COMPUTE_UNITS, sizeof(cl_uint), &compute_units, NULL);
    fprintf(stdout, "Parallel compute units \t: %u\n", compute_units);

    clGetDeviceInfo(device_id, CL_DEVICE_PREFERRED_VECTOR_WIDTH_CHAR, sizeof(char_vector_width), &char_vector_width, NULL);
    fprintf(stdout, "Preferred vector width \t: %u\n", char_vector_width);

    clGetDeviceInfo(device_id, CL_DEVICE_GLOBAL_MEM_SIZE, sizeof(cl_uint), &global_mem_size, NULL);
    fprintf(stdout, "Max object allocation \t: %u bytes\n", global_mem_size);

    clGetDeviceInfo(device_id, CL_DEVICE_GLOBAL_MEM_CACHE_SIZE, sizeof(cl_uint), &global_mem_cache, NULL);
    fprintf(stdout, "Global memory cache \t: %u bytes\n", global_mem_cache);

    clGetDeviceInfo(device_id, CL_DEVICE_MAX_CONSTANT_BUFFER_SIZE, sizeof(cl_uint), &buffer_size, NULL);
    fprintf(stdout, "Constant buffer \t: %u bytes\n", buffer_size);

    clGetDeviceInfo(device_id, CL_DEVICE_LOCAL_MEM_SIZE, sizeof(cl_uint), &local_mem_size, NULL);
    fprintf(stdout, "Local memory region \t: %u bytes\n", local_mem_size);

    clGetDeviceInfo(device_id, CL_DEVICE_MAX_WORK_GROUP_SIZE, sizeof(size_t), &local_size, NULL);
    fprintf(stdout, "Maximum work group size : %zu \n", local_size);

    clGetDeviceInfo(device_id, CL_DEVICE_GLOBAL_MEM_CACHELINE_SIZE, sizeof(cl_uint), &cache_size, NULL);
    fprintf(stdout, "Cacheline size \t\t: %u \n", cache_size);

    clGetDeviceInfo(device_id, CL_DEVICE_MAX_CLOCK_FREQUENCY, sizeof(cl_uint), &clock_freq, NULL);
    fprintf(stdout, "Max clock \t\t: %u MHz\n", clock_freq);

    clGetDeviceInfo(device_id, CL_DEVICE_PROFILING_TIMER_RESOLUTION, sizeof(time_res), &time_res, NULL);
    fprintf(stdout, "Time resolution \t: %zu ns\n", time_res);
    cl_bool image_support = CL_FALSE;
    clGetDeviceInfo(device_id, CL_DEVICE_IMAGE_SUPPORT, sizeof(image_support), &image_support, NULL);
    if (image_support == CL_TRUE) {
        printf("Device Support Image operations!\n");
    } else {
        printf("Device doesn't support Image operations!\n");
    }

    // Query the maximum dimensions for 2D images
    size_t max_width, max_height;
    err = clGetDeviceInfo(device_id, CL_DEVICE_IMAGE2D_MAX_WIDTH, sizeof(size_t), &max_width, NULL);
    if (err != CL_SUCCESS) {
        printf("Error getting max width\n");
    }

    err = clGetDeviceInfo(device_id, CL_DEVICE_IMAGE2D_MAX_HEIGHT, sizeof(size_t), &max_height, NULL);
    if (err != CL_SUCCESS) {
        printf("Error getting max height\n");
    }

    printf("Maximum supported 2D Image Width: %zu\n", max_width);
    printf("Maximum supported 2D Image Height: %zu\n", max_height);

    printf(" --------------- \n");

    err = clGetDeviceInfo(device_id, CL_DEVICE_SINGLE_FP_CONFIG, sizeof(flag), &flag, NULL);
    if (err < 0) {
        perror("Couldn't read device FP information");
    }
    printf("Float Processing Features:\n");
    if (flag & CL_FP_INF_NAN)
        printf("INF and NaN values supported.\n");
    if (flag & CL_FP_DENORM)
        printf("Denormalized numbers supported.\n");
    if (flag & CL_FP_ROUND_TO_NEAREST)
        printf("Round To Nearest Even mode supported.\n");
    if (flag & CL_FP_ROUND_TO_INF)
        printf("Round To Infinity mode supported.\n");
    if (flag & CL_FP_ROUND_TO_ZERO)
        printf("Round To Zero mode supported.\n");
    if (flag & CL_FP_FMA)
        printf("Floating-point multiply-and-add operation supported.\n");

    printf(" ---- Memory Information ---- \n");
    cl_device_local_mem_type mt;
    err = clGetDeviceInfo(device_id, CL_DEVICE_LOCAL_MEM_TYPE, sizeof(cl_device_local_mem_type), &mt, NULL);
    if (err < 0) {
        perror("Couldn't read local memory information");
    } else {
        if (mt == CL_LOCAL) {
            printf("Local memory is a local memory storage.\n");
        } else {
            printf("Local memory is a global memory storage.\n");
        }
    }
    printf("===== end printing device information =====\n\n");
}
