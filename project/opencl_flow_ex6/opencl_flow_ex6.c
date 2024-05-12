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

#define CHECK_DATA_SIZE(ret, i, name)                                                      \
    if (ret > i)                                                                           \
    {                                                                                      \
        fprintf(stderr, "%s is too long for buffer on %s:%d\n", name, __FILE__, __LINE__); \
    }

// This is taken from OpenCL SDK
#define OCLERROR_RET(func, err, label)                                                  \
    do                                                                                  \
    {                                                                                   \
        err = func;                                                                     \
        if (err != CL_SUCCESS)                                                          \
        {                                                                               \
            fprintf(stderr, "on line %d, in file %s\n%s\n", __LINE__, __FILE__, #func); \
            goto label;                                                                 \
        }                                                                               \
    } while (0)

#define CHECK_ERR(err, label, msg)                                                       \
    if (err != CL_SUCCESS)                                                          \
    {                                                                               \
        fprintf(stderr, "%s, on line %d, in file %s\n", msg, __LINE__, __FILE__); \
        goto label;                                                                 \
    }


void apply_occlusion_fill_6(cl_device_id device, cl_context context, cl_kernel kernel, cl_command_queue queue, const Image *im0, Image *output_im0)
{
    /* Image data */
    cl_mem input_image, output_image;
    cl_image_format input_format, output_format;
    cl_int err;

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
    CHECK_ERR(err, end, "occlustion_fill: Couldn't create the input image 0 object")

    /* Create output image object */
    output_image = clCreateImage2D(context, CL_MEM_READ_WRITE | CL_MEM_ALLOC_HOST_PTR, &output_format, width, height, 0, NULL, &err);
    CHECK_ERR(err, end, "occlustion_fill: Couldn't create the input image object")

    // Set kernel arguments
    OCLERROR_RET(clSetKernelArg(kernel, 0, sizeof(cl_mem), &input_image), err, end);

    OCLERROR_RET(clSetKernelArg(kernel, 1, sizeof(cl_mem), &output_image), err, end);

    // Execute the OpenCL kernel
    size_t globalWorkSize[2] = { width, height };
//    const size_t workSize = 5;
//    size_t localWorkSize[2] = { workSize, workSize };
    err = clEnqueueNDRangeKernel(queue, kernel, 2, NULL, globalWorkSize, NULL, 0, NULL, &occlustion_fill_event);
    if (err != CL_SUCCESS)
    {
        fprintf(stderr, "Error: Failed to execute kernel %d !\n", err);
        goto end;
    }

    // Read the output image back to the host
    OCLERROR_RET(clEnqueueReadImage(queue,
                   output_image,
                   CL_TRUE,
                   (size_t[3]){ 0, 0, 0 },
                   (size_t[3]){ width, height, 1 },
                   0,
                   0,
                   (void *)output_im0->image,
                   0,
                   NULL,
                   &occlustion_fill_read_event),
      err,
      end);

    size_t wg_size, wg_multiple;
    cl_ulong private_usage;
    cl_ulong local_usage;
    size_t param_value_size_ret;
    /* Access kernel/work-group properties */
    OCLERROR_RET(clGetKernelWorkGroupInfo(kernel, device, CL_KERNEL_WORK_GROUP_SIZE, sizeof(wg_size), &wg_size, &param_value_size_ret), err, end);
    CHECK_DATA_SIZE(param_value_size_ret, sizeof(wg_size), "CL_KERNEL_WORK_GROUP_SIZE")
    OCLERROR_RET(clGetKernelWorkGroupInfo(kernel, device, CL_KERNEL_PREFERRED_WORK_GROUP_SIZE_MULTIPLE, sizeof(wg_multiple), &wg_multiple,
      &param_value_size_ret), err, end);
    CHECK_DATA_SIZE(param_value_size_ret, sizeof(wg_multiple), "CL_KERNEL_PREFERRED_WORK_GROUP_SIZE_MULTIPLE")
    OCLERROR_RET(clGetKernelWorkGroupInfo(kernel, device, CL_KERNEL_LOCAL_MEM_SIZE, sizeof(local_usage), &local_usage, &param_value_size_ret), err, end);
    CHECK_DATA_SIZE(param_value_size_ret, sizeof(local_usage), "CL_KERNEL_LOCAL_MEM_SIZE")
    OCLERROR_RET(clGetKernelWorkGroupInfo(kernel, device, CL_KERNEL_PRIVATE_MEM_SIZE, sizeof(private_usage), &private_usage, &param_value_size_ret), err, end);
    CHECK_DATA_SIZE(param_value_size_ret, sizeof(private_usage), "CL_KERNEL_PRIVATE_MEM_SIZE")

    printf("The maximum work-group size is %zu and the work-group multiple is %zu.\n\n", wg_size, wg_multiple);
    printf("The kernel uses %zu bytes of local memory. It uses %zu bytes of private memory.\n",
      local_usage, private_usage);


    clFinish(queue);

    output_im0->width = width;
    output_im0->height = height;

    time_to_occlustion_fill = getExecutionTime(occlustion_fill_event);
    read_time = getExecutionTime(occlustion_fill_read_event);

    clReleaseEvent(occlustion_fill_read_event);
    clReleaseEvent(occlustion_fill_event);

end:
    printf("Time taken to do the occlustion_fill = %llu ns\n", time_to_occlustion_fill);
    printf("Time taken to read the output image (occlustion_fill) = %llu ns\n", read_time);
}

cl_program build_program_6(cl_context ctx, cl_device_id device, const char* filename) {

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

    err = clBuildProgram(program, 0, NULL, "-cl-denorms-are-zero", NULL, NULL);
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
    if (err < 0)
    {
        perror("Error: clCreateContext");
        exit(1);
    }

    program = build_program_6(context, device, PROGRAM_FILE);

    /* Find out how many kernels are in the source file */
    err = clCreateKernelsInProgram(program, 0, NULL, &num_kernels);
    if (err < 0)
    {
        perror("Couldn't find any kernels");
        exit(1);
    }

    printf("Number of kernels: %u\n", num_kernels);

    /* Create a kernel for each function */
    kernels = (cl_kernel *)malloc(num_kernels * sizeof(cl_kernel));
    clCreateKernelsInProgram(program, num_kernels, kernels, NULL);

    // /* Search for the named kernel */
    for (i = 0; i < num_kernels; i++)
    {
        clGetKernelInfo(kernels[i], CL_KERNEL_FUNCTION_NAME, sizeof(kernel_name), kernel_name, NULL);
        if (strcmp(kernel_name, KERNEL_RESIZE_IMAGE) == 0)
        {
            kernel_resize_image = kernels[i];
            printf("Found resize_image kernel at index %u.\n", i);
        } else if (strcmp(kernel_name, KERNEL_COLOR_TO_GRAY) == 0)
        {
            kernel_color_to_gray = kernels[i];
            printf("Found color_to_gray kernel at index %u.\n", i);
        } else if (strcmp(kernel_name, KERNEL_ZNCC_LEFT) == 0)
        {
            kernel_zncc_left = kernels[i];
            printf("Found zncc_left kernel at index %u.\n", i);
        } else if (strcmp(kernel_name, KERNEL_ZNCC_RIGHT) == 0)
        {
            kernel_zncc_right = kernels[i];
            printf("Found zncc_right kernel at index %u.\n", i);
        } else if (strcmp(kernel_name, KERNEL_CROSS_CHECK) == 0)
        {
            kernel_cross_check = kernels[i];
            printf("Found cross_check kernel at index %u.\n", i);
        } else if (strcmp(kernel_name, KERNEL_OCCLUSION_FILL) == 0)
        {
            kernel_occlusion_fill = kernels[i];
            printf("Found occlustion kernel at index %u.\n", i);
        }
    }
    // cl_command_queue_properties props[3] = {CL_QUEUE_PROPERTIES, CL_QUEUE_PROFILING_ENABLE, 0};
    // queue = clCreateCommandQueueWithProperties(context, device, props, &err);
    queue = clCreateCommandQueue(context, device, CL_QUEUE_PROFILING_ENABLE, &err);
    if (err < 0)
    {
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
    apply_occlusion_fill_6(device, context, kernel_occlusion_fill, queue, left_crosscheck_im0, output_left_occlusion_im0);

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

void printDeviceInformation()
{
    cl_device_id device = create_device();
    printDeviceInformationHelper(device);
}

void printDeviceInformationHelper(cl_device_id device_id)
{
    printf("############ ===== start printing device information ===== ############\n\n");
    cl_device_fp_config flag;
    cl_platform_id platform_id;
    char buf[128];
    char p_name[40];
    char p_vendor[40];
    cl_long time_res, local_size;
    cl_int err;
    cl_uint work_item_dim, compute_units, char_vector_width, cache_size, clock_freq;
    size_t param_value_size_ret;

    OCLERROR_RET(clGetDeviceInfo(device_id, CL_DEVICE_PLATFORM, sizeof(cl_platform_id), &platform_id, &param_value_size_ret), err, end);
    CHECK_DATA_SIZE(param_value_size_ret, sizeof(cl_platform_id), "Platform ID")

    OCLERROR_RET(clGetPlatformInfo(platform_id, CL_PLATFORM_NAME, 40, p_name, &param_value_size_ret), err, end);
    CHECK_DATA_SIZE(param_value_size_ret, 40, "Platform name")
    printf("Platform name\t\t: %s\n", p_name);

    OCLERROR_RET(clGetPlatformInfo(platform_id, CL_PLATFORM_VENDOR, 40, p_vendor, &param_value_size_ret), err, end);
    CHECK_DATA_SIZE(param_value_size_ret, 40, "Platform vendor")
    printf("Platform vendor\t\t: %s\n", p_vendor);

    OCLERROR_RET(clGetDeviceInfo(device_id, CL_DEVICE_NAME, 128, buf, &param_value_size_ret), err, end);
    CHECK_DATA_SIZE(param_value_size_ret, 128, "Device name")
    printf("Device \t\t\t: %s\n", buf);

    OCLERROR_RET(clGetDeviceInfo(device_id, CL_DEVICE_VERSION, 128, buf, &param_value_size_ret), err, end);
    CHECK_DATA_SIZE(param_value_size_ret, 128, "Device version")
    printf("OpenCL version \t\t: %s\n", buf);

    OCLERROR_RET(clGetDeviceInfo(device_id, CL_DEVICE_MAX_WORK_ITEM_DIMENSIONS, sizeof(cl_uint), &work_item_dim, &param_value_size_ret), err, end);
    CHECK_DATA_SIZE(param_value_size_ret, sizeof(cl_uint), "Device version")
    printf("Work dimensions \t: %u\n", work_item_dim);

    OCLERROR_RET(clGetDeviceInfo(device_id, CL_DEVICE_MAX_COMPUTE_UNITS, sizeof(cl_uint), &compute_units, &param_value_size_ret), err, end);
    CHECK_DATA_SIZE(param_value_size_ret, sizeof(cl_uint), "Device Parallel compute units")
    printf("Parallel compute units \t: %u\n", compute_units);

    OCLERROR_RET(
      clGetDeviceInfo(device_id, CL_DEVICE_PREFERRED_VECTOR_WIDTH_CHAR, sizeof(char_vector_width), &char_vector_width, &param_value_size_ret), err, end);
    CHECK_DATA_SIZE(param_value_size_ret, sizeof(char_vector_width), "Preferred vector width")
    printf("Preferred vector width \t: %u\n", char_vector_width);

    cl_long global_mem_alloc_size;
    OCLERROR_RET(
      clGetDeviceInfo(device_id, CL_DEVICE_MAX_MEM_ALLOC_SIZE, sizeof(global_mem_alloc_size), &global_mem_alloc_size, &param_value_size_ret), err, end);
    CHECK_DATA_SIZE(param_value_size_ret, sizeof(global_mem_alloc_size), "Max memory allocation")
    printf("Max memory allocation \t: %lld bytes\n", global_mem_alloc_size);

    cl_long global_mem_size;
    OCLERROR_RET(clGetDeviceInfo(device_id, CL_DEVICE_GLOBAL_MEM_SIZE, sizeof(cl_long), &global_mem_size, &param_value_size_ret), err, end);
    CHECK_DATA_SIZE(param_value_size_ret, sizeof(cl_long), "Global memory siz")
    printf("Global memory size \t: %lld bytes\n", global_mem_size);

    cl_long global_mem_cache;
    OCLERROR_RET(clGetDeviceInfo(device_id, CL_DEVICE_GLOBAL_MEM_CACHE_SIZE, sizeof(cl_long), &global_mem_cache, &param_value_size_ret), err, end);
    CHECK_DATA_SIZE(param_value_size_ret, sizeof(cl_long), "Global memory cache")
    printf("Global memory cache \t: %lld bytes\n", global_mem_cache);

    cl_long buffer_size;
    OCLERROR_RET(clGetDeviceInfo(device_id, CL_DEVICE_MAX_CONSTANT_BUFFER_SIZE, sizeof(cl_long), &buffer_size, &param_value_size_ret), err, end);
    CHECK_DATA_SIZE(param_value_size_ret, sizeof(cl_long), "Constant buffer")
    printf("Constant buffer \t: %lld bytes\n", buffer_size);

    cl_long local_mem_size_16;
    OCLERROR_RET(clGetDeviceInfo(device_id, CL_DEVICE_LOCAL_MEM_SIZE, sizeof(cl_long), &local_mem_size_16, &param_value_size_ret), err, end);
    CHECK_DATA_SIZE(param_value_size_ret, sizeof(cl_long), "Local memory size")
    printf("Local memory size \t: %lld bytes\n", local_mem_size_16);

    OCLERROR_RET(clGetDeviceInfo(device_id, CL_DEVICE_MAX_WORK_GROUP_SIZE, sizeof(local_size), &local_size, &param_value_size_ret), err, end);
    CHECK_DATA_SIZE(param_value_size_ret, sizeof(local_size), "Maximum work group size")
    printf("Maximum work group size : %zu \n", local_size);

    OCLERROR_RET(clGetDeviceInfo(device_id, CL_DEVICE_GLOBAL_MEM_CACHELINE_SIZE, sizeof(cl_uint), &cache_size, &param_value_size_ret), err, end);
    CHECK_DATA_SIZE(param_value_size_ret, sizeof(cl_uint), "Cache line size")
    printf("Cache line size \t: %u \n", cache_size);

    OCLERROR_RET(clGetDeviceInfo(device_id, CL_DEVICE_MAX_CLOCK_FREQUENCY, sizeof(cl_uint), &clock_freq, &param_value_size_ret), err, end);
    CHECK_DATA_SIZE(param_value_size_ret, sizeof(cl_uint), "Max clock")
    printf("Max clock \t\t: %u MHz\n", clock_freq);

    OCLERROR_RET(clGetDeviceInfo(device_id, CL_DEVICE_PROFILING_TIMER_RESOLUTION, sizeof(time_res), &time_res, &param_value_size_ret), err, end);
    CHECK_DATA_SIZE(param_value_size_ret, sizeof(time_res), "Time resolution")
    printf("Time resolution \t: %zu ns\n", time_res);

    cl_bool image_support = CL_FALSE;
    OCLERROR_RET(clGetDeviceInfo(device_id, CL_DEVICE_IMAGE_SUPPORT, sizeof(image_support), &image_support, &param_value_size_ret), err, end);
    CHECK_DATA_SIZE(param_value_size_ret, sizeof(image_support), "Device Support Image")
    if (image_support == CL_TRUE)
    {
        printf("Device Support \t\t: Image operations!\n");
    } else
    {
        printf("Device doesn't support Image operations!\n");
    }

    // Query the maximum dimensions for 2D images
    size_t max_width, max_height;
    OCLERROR_RET(clGetDeviceInfo(device_id, CL_DEVICE_IMAGE2D_MAX_WIDTH, sizeof(max_width), &max_width, &param_value_size_ret), err, end);
    CHECK_DATA_SIZE(param_value_size_ret, sizeof(max_width), "Maximum supported 2D Image Width")
    printf("Maximum 2D Image Width\t: %zu\n", max_width);

    OCLERROR_RET(clGetDeviceInfo(device_id, CL_DEVICE_IMAGE2D_MAX_HEIGHT, sizeof(max_height), &max_height, &param_value_size_ret), err, end);
    CHECK_DATA_SIZE(param_value_size_ret, sizeof(max_height), "Maximum supported 2D Image height")
    printf("Maximum 2D Image Height\t: %zu\n", max_height);

    OCLERROR_RET(clGetDeviceInfo(device_id, CL_DEVICE_SINGLE_FP_CONFIG, sizeof(flag), &flag, &param_value_size_ret), err, end);
    printf(" * Float Processing Features:\n");
    if (flag & CL_FP_INF_NAN)
        printf("Supports \t: INF and NaN values\n");
    if (flag & CL_FP_DENORM)
        printf("Supports \t: Denormalized numbers\n");
    if (flag & CL_FP_ROUND_TO_NEAREST)
        printf("Supports \t: Round To Nearest Even mode\n");
    if (flag & CL_FP_ROUND_TO_INF)
        printf("Supports \t: Round To Infinity mode\n");
    if (flag & CL_FP_ROUND_TO_ZERO)
        printf("Supports \t: Round To Zero mode\n");
    if (flag & CL_FP_FMA)
        printf("Supports \t: Floating-point multiply-and-add operation\n");
    if (flag & CL_FP_SOFT_FLOAT)
        printf("Supports \t: CL_FP_SOFT_FLOAT operation\n");
    if (flag & CL_FP_CORRECTLY_ROUNDED_DIVIDE_SQRT)
        printf("Supports \t: CL_FP_CORRECTLY_ROUNDED_DIVIDE_SQRT operation\n");
end:
    printf("\n############ ===== END printing device information ===== ############\n\n");
}
