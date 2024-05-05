//
// Created by ruksh on 28/02/2024.
//
#include "util.h"
#include <assert.h>
#include <math.h>
#include <stdlib.h>

ProcessTime *createProcessTime(int numSamples)
{
    ProcessTime *processTime = (ProcessTime *)malloc(sizeof(ProcessTime));
    processTime->elapsedTimes = (float *)malloc(sizeof(float) * numSamples);
    processTime->numSamples = numSamples;
    processTime->averageElapsedTime = 0;
    return processTime;
}

ProfileInformation *createProfileInformation(int initialSamples)
{
    ProfileInformation *profileInformation = (ProfileInformation *)malloc(sizeof(ProfileInformation));
    profileInformation->readImage = createProcessTime(initialSamples);
    profileInformation->resizeImage = createProcessTime(initialSamples);
    profileInformation->grayScaleImage = createProcessTime(initialSamples);
    profileInformation->applyFilter = createProcessTime(initialSamples);
    profileInformation->saveImage = createProcessTime(initialSamples);
    profileInformation->filter = createProcessTime(initialSamples);
    profileInformation->zncc_left = createProcessTime(initialSamples);
    profileInformation->zncc_right = createProcessTime(initialSamples);
    profileInformation->crossCheck = createProcessTime(initialSamples);
    profileInformation->occlusion = createProcessTime(initialSamples);
    return profileInformation;
}

void reinitProcessTime(ProcessTime *processTime, int numSamples)
{
    freeProcessTime(processTime);
    processTime = createProcessTime(numSamples);
}

void freeProfileInformation(ProfileInformation *profileInformation)
{
    freeProcessTime(profileInformation->readImage);
    freeProcessTime(profileInformation->resizeImage);
    freeProcessTime(profileInformation->grayScaleImage);
    freeProcessTime(profileInformation->applyFilter);
    freeProcessTime(profileInformation->saveImage);
    freeProcessTime(profileInformation->filter);
    freeProcessTime(profileInformation->zncc_left);
    freeProcessTime(profileInformation->zncc_right);
    freeProcessTime(profileInformation->crossCheck);
    freeProcessTime(profileInformation->occlusion);
    free(profileInformation);
}

void freeProcessTime(ProcessTime *processTime)
{
    free(processTime->elapsedTimes);
    free(processTime);
}

int checkTimes(ProcessTime *processTime)
{
    int numberOfSamples = processTime->numSamples;
    float mean = Average(processTime->elapsedTimes, numberOfSamples);
    float sd = standardDeviation(processTime->elapsedTimes, numberOfSamples);
    int req_n = requiredSampleSize(sd, mean);
    if (req_n > numberOfSamples) {
        increaseSampleSize(processTime, req_n);
        return 0;
    }
    processTime->averageElapsedTime = mean;
    return 1;
}

void increaseSampleSize(ProcessTime *processTime, int numSamples)
{
    processTime->elapsedTimes = (float *)realloc(processTime->elapsedTimes, sizeof(float) * numSamples);
}

long requiredSampleSize(float sd, float mean)
{
    long N = (long)ceil(((float)100 * 1.960 * sd) / (5 * mean));
    return N;
}

float Average(const float *times, const int numSamples)
{
    float sum = 0;
    for (int i = 0; i < numSamples; ++i) { sum += times[i]; }
    return (double)sum / numSamples;
}

float standardDeviation(float times[], int numSamples)
{
    float u = Average(times, numSamples);
    float variance = 0;
    for (int i = 0; i < numSamples; ++i) { variance += pow(times[i] - u, 2); }
    variance = variance / numSamples;
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
