//
// Created by ruksh on 28/02/2024.
//

#ifndef MULTIPROCOPENCL_DRIVER_H
#define MULTIPROCOPENCL_DRIVER_H
#include <pngloader.h>

enum BENCHMARK {  DO_NOT_BENCHMARK, BENCHMARK };
typedef enum BENCHMARK BENCHMARK_MODE;

void runZnccFlowForOneImage(const char * imagePath, const char * outputPath);
void zncc_flow_driver(const char *imagePath, const char *outputPath);
void createSampleTestPng();
Image* getFilterdBWImage(const char * imagePath, const char * outputPath);
void fullFlow(BENCHMARK_MODE benchmarking, bool multiThreadedMode);
void fullFlow_MT();
void postProcessFlow();


// private methods
Image *resizeImageDriver(Image *inputImage, BENCHMARK_MODE benchmarkMode, ProfileInformation *pInformation);
Image *grayScaleImageDriver(Image *inputImage, BENCHMARK_MODE benchmarkMode, ProfileInformation *pInformation);
void grayScaleImageDriverTimes(Image *inputImage, ProfileInformation *profileInformation);
void saveImageDriver(const char *fileName, Image *inputImage, BENCHMARK_MODE benchmarkMode, ProfileInformation *pInformation);
void saveImageTimes(const char *name, Image *inputImage, ProfileInformation *pInformation);

#endif //MULTIPROCOPENCL_DRIVER_H
