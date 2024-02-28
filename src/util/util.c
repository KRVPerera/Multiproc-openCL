//
// Created by ruksh on 28/02/2024.
//
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <getopt.h>
#include "util.h"

long requiredSampleSize(float sd, float mean) {
    long N = (long) ceil(((float) 100 * 1.960 * sd) / (5 * mean));
    return N;
}

float Average(float *times, int numSamples) {
    float sum = 0;
    for (int i = 0; i < numSamples; ++i) {
        sum += times[i];
    }
    return (double) sum / numSamples;
}

float standardDeviation(float times[], int numSamples) {
    float u = Average(times, numSamples);
    float variance = 0;
    for (int i = 0; i < numSamples; ++i) {
        variance += pow(times[i] - u, 2);
    }
    variance = variance / numSamples;
    return sqrt(variance);
}

float elapsed_time_microsec(struct timespec *begin, struct timespec *end,
                            unsigned long *sec, unsigned long *nsec) {

    // make sure end time is after the begin
    assert(end->tv_sec > begin->tv_sec ||
           (end->tv_sec == begin->tv_sec &&
            end->tv_nsec >= begin->tv_nsec));

    if (end->tv_nsec < begin->tv_nsec) {
        *nsec = 1000000000 - (begin->tv_nsec - end->tv_nsec);
        *sec = end->tv_sec - begin->tv_sec - 1;
    } else {
        *nsec = end->tv_nsec - begin->tv_nsec;
        *sec = end->tv_sec - begin->tv_sec;
    }
    return (float) (*sec) * 1000 + ((float) (*nsec)) * 1E-6;
}