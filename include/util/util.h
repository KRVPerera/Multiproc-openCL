//
// Created by ruksh on 28/02/2024.
//

#ifndef MULTIPROCOPENCL_UTIL_H
#define MULTIPROCOPENCL_UTIL_H

#include <time.h>

float elapsed_time_microsec(struct timespec *begin, struct timespec *end,
                            unsigned long *sec, unsigned long *nsec);
float standardDeviation(float times[], int numSamples);
float Average(float *times, int numSamples);
long requiredSampleSize(float sd, float mean);

#endif //MULTIPROCOPENCL_UTIL_H
