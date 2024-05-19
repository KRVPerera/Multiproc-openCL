//
// Created by ruksh on 28/02/2024.
//
#include "util.h"
#include <assert.h>
#include <logger.h>
#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include <omp.h>
#include <unistd.h>
#include <time.h>
#include <calc_pi.h>

ProcessTime *createProcessTime(int numSamples)
{
    ProcessTime *processTime = (ProcessTime *)malloc(sizeof(ProcessTime));
    processTime->elapsedTimes = (float *)malloc(sizeof(float) * numSamples);
    processTime->numSamples = numSamples;
    processTime->averageElapsedTime = 0;
    processTime->averageCalculated = 0;
    return processTime;
}

ProfileInformation *createProfileInformationWithoutBenchmarking()
{
    ProfileInformation *profileInformation = (ProfileInformation *)malloc(sizeof(ProfileInformation));
    profileInformation->multiThreaded = false;
    profileInformation->readImage = createProcessTime(1);
    profileInformation->resizeImage = createProcessTime(1);
    profileInformation->grayScaleImage = createProcessTime(1);
    profileInformation->applyFilter = createProcessTime(1);
    profileInformation->saveImage = createProcessTime(1);
    profileInformation->zncc_left = createProcessTime(1);
    profileInformation->zncc_right = createProcessTime(1);
    profileInformation->crossCheck = createProcessTime(1);
    profileInformation->occlusion = createProcessTime(1);
    return profileInformation;
}

ProfileInformation *createProfileInformation(int initialSamples)
{
    ProfileInformation *profileInformation = (ProfileInformation *)malloc(sizeof(ProfileInformation));
    profileInformation->multiThreaded = false;
    profileInformation->readImage = createProcessTime(initialSamples);
    profileInformation->resizeImage = createProcessTime(initialSamples);
    profileInformation->grayScaleImage = createProcessTime(initialSamples);
    profileInformation->applyFilter = createProcessTime(initialSamples);
    profileInformation->saveImage = createProcessTime(initialSamples);
    profileInformation->zncc_left = createProcessTime(initialSamples);
    profileInformation->zncc_right = createProcessTime(initialSamples);
    profileInformation->crossCheck = createProcessTime(initialSamples);
    profileInformation->occlusion = createProcessTime(initialSamples);
    return profileInformation;
}

void printSummary(ProfileInformation *pInformation)
{
    logger("Summary of the benchmarking results");
    if (pInformation->readImage->averageCalculated)
        logger("Image Load Time \t: %.3f ms", pInformation->readImage->averageElapsedTime);
    if (pInformation->resizeImage->averageCalculated)
        logger("Image Resize Time \t: %.3f ms", pInformation->resizeImage->averageElapsedTime);
    if (pInformation->grayScaleImage->averageCalculated)
        logger("Image Grayscale Time \t: %.3f ms", pInformation->grayScaleImage->averageElapsedTime);
    if (pInformation->applyFilter->averageCalculated)
        logger("Image applyFilter Time : %.3f ms", pInformation->applyFilter->averageElapsedTime);
    if (pInformation->saveImage->averageCalculated)
        logger("Image Save Time \t: %.3f ms", pInformation->saveImage->averageElapsedTime);
    if (pInformation->zncc_left->averageCalculated)
        logger("Left Disparity Time \t: %.3f ms", pInformation->zncc_left->averageElapsedTime);
    if (pInformation->zncc_right->averageCalculated)
        logger("Right Disparity Time \t: %.3f ms", pInformation->zncc_right->averageElapsedTime);
    if (pInformation->crossCheck->averageCalculated)
        logger("Cross Check Time \t: %.3f ms", pInformation->crossCheck->averageElapsedTime);
    if (pInformation->occlusion->averageCalculated)
        logger("Occlusion Fill Time \t: %.3f ms", pInformation->occlusion->averageElapsedTime);
}

void reinitProcessTime(ProcessTime **processTime, int numSamples)
{
    freeProcessTime(*processTime);
    *processTime = createProcessTime(numSamples);
}

void freeProfileInformation(ProfileInformation *profileInformation)
{
    freeProcessTime(profileInformation->readImage);
    freeProcessTime(profileInformation->resizeImage);
    freeProcessTime(profileInformation->grayScaleImage);
    freeProcessTime(profileInformation->applyFilter);
    freeProcessTime(profileInformation->saveImage);
    freeProcessTime(profileInformation->zncc_left);
    freeProcessTime(profileInformation->zncc_right);
    freeProcessTime(profileInformation->crossCheck);
    freeProcessTime(profileInformation->occlusion);
    free(profileInformation);
}

void freeProcessTime(ProcessTime *processTime)
{
    assert(processTime != NULL);
    free(processTime->elapsedTimes);
    free(processTime);
}

int checkTimes(ProcessTime *processTime)
{
    processTime->averageCalculated = false;
    processTime->averageElapsedTime = 0;
    int numberOfSamples = processTime->numSamples;
    float mean = Average(processTime->elapsedTimes, numberOfSamples);
    float sd = standardDeviation(processTime->elapsedTimes, numberOfSamples);
    int req_n = requiredSampleSize(sd, mean);
    if (req_n > numberOfSamples) {
        increaseSampleSize(processTime, req_n);
        logger("Increasing sample size to %d", req_n);
        processTime->averageCalculated = false;
        return 0;
    }
    processTime->averageElapsedTime = mean;
    processTime->averageCalculated = true;
    return 1;
}

void increaseSampleSize(ProcessTime *processTime, int numSamples)
{
    processTime->elapsedTimes = (float *)realloc(processTime->elapsedTimes, sizeof(float) * numSamples);
    processTime->numSamples = numSamples;
}

long requiredSampleSize(float sd, float mean)
{
    long N = (long)ceil(((float)100 * 1.960 * sd) / (5 * mean));
    return N;
}

float Average(const float *times, int numSamples)
{
    float sum = 0.0f;
    for (int i = 0; i < numSamples; ++i) { sum += times[i]; }
    return sum / (float) numSamples;
}

float standardDeviation(float times[], int numSamples)
{
    float u = Average(times, numSamples);
    float variance = 0;
    for (int i = 0; i < numSamples; ++i) { variance += (float) pow(times[i] - u, 2); }
    variance = variance / (float) numSamples;
    return sqrt(variance);
}

float elapsed_time_microsec(struct timespec *begin, struct timespec *end, unsigned long *sec, unsigned long *nsec)
{

    // make sure end time is after the begin
    assert(end->tv_sec > begin->tv_sec || (end->tv_sec == begin->tv_sec && end->tv_nsec >= begin->tv_nsec));

    if (end->tv_nsec < begin->tv_nsec) {
        *nsec = 1000000000 - (begin->tv_nsec - end->tv_nsec);
        *sec = end->tv_sec - begin->tv_sec - 1;
    } else {
        *nsec = end->tv_nsec - begin->tv_nsec;
        *sec = end->tv_sec - begin->tv_sec;
    }
    return (float)(*sec) * 1000000 + ((float)(*nsec)) * 1E-3;
}

void printHelp(void) {
    printf("\nHow to use the program\n");
    printf("Usage \t\t: <opencl|mp|single|test|opencl_platform|opencl_opt|opencl>\n");
    printf("opencl \t\t: Run the occlusion filling using OpenCL\n");
    printf("opencl_opt \t: Run the optimized opencl version\n");
    printf("mp [-benchmark] : Run all using multithreading mode. Use -benchmark for benchmarking\n");
    printf("single [-benchmark] : Run all using a single thread. Use -benchmark for benchmarking\n");
    printf("test \t\t: Run the test code for OpenMP\n");
    printf("opencl_platform : Print the OpenCL platform information\n\n");
}

void printHeader(void) {
    printf("#################################################\n");
    printf("###### Multiprocessor Programming project! ######\n");
    printf("###### by Dilan Fernando & Rukshan Perera  ######\n");
    printf("#################################################\n");
}

void openmpTestCode(void)
{
    logger("Running multithreaded mode.");
    int maxthreads = omp_get_max_threads();

    printf("Max threads: %d\n", maxthreads);

//    #pragma omp parallel num_threads(5)
#pragma omp parallel
    {
        int id = omp_get_thread_num();
        printf("Hello %d\n", id);
        sleep(1);
        printf("World %d\n", id);
    }

    int num_steps = 1000000000;
    struct timespec t0, t1;
    unsigned long sec, nsec;
    GET_TIME(t0)
    double pi = calc_pi(num_steps);
    GET_TIME(t1)
    logger("Pi: %f", pi);
    float elapsed_time = elapsed_time_microsec(&t0, &t1, &sec, &nsec);
    logger("calc_pi(%d) time : %f micro seconds", num_steps, elapsed_time);

    GET_TIME(t0)
    pi = calc_pi_mt(num_steps);
    GET_TIME(t1)
    logger("MT Pi: %f", pi);
    elapsed_time = elapsed_time_microsec(&t0, &t1, &sec, &nsec);
    logger("calc_pi_mt(%d) time : %f micro seconds", num_steps, elapsed_time);
}
