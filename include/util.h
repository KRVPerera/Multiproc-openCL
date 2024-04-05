#ifndef MULTIPROCOPENCL_UTIL_H
#define MULTIPROCOPENCL_UTIL_H

#include <time.h>

typedef enum Mode {
    OPENMP,
    OPENCL,
    MULTIPROCESS,
    UNKNOWN
} Mode;

struct TimesForStages {
    Mode mode;
    char* stageName;
    float *times;
    int numSamples;
};

typedef struct AllStages {
    struct TimesForStages *stages;
    int numStages;
} AllStages;

#define GET_TIME(x); if(clock_gettime(CLOCK_MONOTONIC, &(x)) < 0) \
{perror("clock_gettime(): "); exit(EXIT_FAILURE);}

float elapsed_time_microsec(struct timespec *begin, struct timespec *end,
                            unsigned long *sec, unsigned long *nsec);
float standardDeviation(float times[], int numSamples);
float Average(const float *times, const int numSamples);
long requiredSampleSize(float sd, float mean);

#endif //MULTIPROCOPENCL_UTIL_H
