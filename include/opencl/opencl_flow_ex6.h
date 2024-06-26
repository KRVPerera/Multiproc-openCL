//
// Created by ruksh on 02/03/2024.
//

#ifndef MULTIPROCOPENCL_OPENCL_FLOW_EX6_H
#define MULTIPROCOPENCL_OPENCL_FLOW_EX6_H
#include <opencl_include.h>

void printDeviceInformation();
void openclFlowEx6(BENCHMARK_MODE benchmark);
void apply_occlusion_fill_6(cl_device_id device, cl_context context, cl_kernel kernel, cl_command_queue queue, const Image *im0, Image *output_im0, BENCHMARK_MODE benchmark);
void printDeviceInformationHelper(cl_device_id device);

#endif //MULTIPROCOPENCL_OPENCL_FLOW_EX6_H
