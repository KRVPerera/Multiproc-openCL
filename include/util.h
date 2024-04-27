#ifndef MULTIPROCOPENCL_UTIL_H
#define MULTIPROCOPENCL_UTIL_H

#include <time.h>

#define GET_TIME(x) if(clock_gettime(CLOCK_MONOTONIC, &(x)) < 0) {perror("clock_gettime(): "); exit(EXIT_FAILURE);}

/**
 * Calculate the elapsed time between two timespec structs
 * @param begin
 * @param end
 * @param sec
 * @param nsec
 * @return
 */
float elapsed_time_microsec(struct timespec *begin, struct timespec *end,
                            unsigned long *sec, unsigned long *nsec);

/**
 * Calculate the standard deviation of a set of samples
 * @param times
 * @param numSamples
 * @return
 */
float standardDeviation(float times[], int numSamples);

/**
 * Calculate the average of a set of samples
 * @param times
 * @param numSamples
 * @return
 */
float Average(const float *times, const int numSamples);

/**
 * Calculate the required sample size for a given standard deviation and mean
 * @param sd
 * @param mean
 * @return
 */
long requiredSampleSize(float sd, float mean);

#endif //MULTIPROCOPENCL_UTIL_H
