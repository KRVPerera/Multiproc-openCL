#ifndef MULTIPROCOPENCL_UTIL_H
#define MULTIPROCOPENCL_UTIL_H

#include <stdbool.h>
#include <time.h>

#define GET_TIME(x)                               \
    if (clock_gettime(CLOCK_MONOTONIC, &(x)) < 0) \
    {                                             \
        perror("clock_gettime(): ");              \
        exit(EXIT_FAILURE);                       \
    }

enum AVERAGE_CALCULATED { FALSE, TRUE };
typedef enum AVERAGE_CALCULATED AVERAGE_CALCULATED;

typedef struct ProcessTime
{
    float *elapsedTimes;
    float averageElapsedTime;
    int numSamples;
    bool averageCalculated;
} ProcessTime;

typedef struct ProfileInformation
{
    bool multiThreaded;
    ProcessTime *readImage;
    ProcessTime *resizeImage;
    ProcessTime *grayScaleImage;
    ProcessTime *applyFilter;
    ProcessTime *saveImage;
    ProcessTime *zncc_left;
    ProcessTime *zncc_right;
    ProcessTime *crossCheck;
    ProcessTime *occlusion;
} ProfileInformation;

/**
 * Check if the number of samples is sufficient for the given standard deviation and mean
 * TODO: function does two things,
 *  - check if the number of samples is sufficient
 *  - increase the sample size if necessary
 * @param processTime
 */
int checkTimes(ProcessTime *processTime);

/**
 * Create a new ProfileInformation struct
 * @param initialSamples
 * @return
 */
ProfileInformation *createProfileInformation(int initialSamples);

/**
 * Create a new ProfileInformation struct without benchmarking
 * @return
 */
ProfileInformation *createProfileInformationWithoutBenchmarking();

/**
 * Free the memory allocated for a ProfileInformation struct
 * @param profileInformation
 */
void freeProfileInformation(ProfileInformation *profileInformation);

/**
 * Create a new ProcessTime struct
 * @param numSamples
 * @return
 */
ProcessTime *createProcessTime(int numSamples);


/**
 * Reinitialize a ProcessTime struct
 * @param processTime
 * @param numSamples
 */
void reinitProcessTime(ProcessTime **processTime, int numSamples);

/**
 * Free the memory allocated for a ProcessTime struct
 * @param processTime
 */
void freeProcessTime(ProcessTime *processTime);

/** Increase sample size of ProcessTime struct
 * @param processTime
 * @param numSamples
 */
void increaseSampleSize(ProcessTime *processTime, int numSamples);

/**
 * Calculate the elapsed time between two timespec structs
 * @param begin
 * @param end
 * @param sec
 * @param nsec
 * @return
 */
float elapsed_time_microsec(struct timespec *begin, struct timespec *end, unsigned long *sec, unsigned long *nsec);

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

/**
 * Print a summary of the ProfileInformation struct
 * @param pInformation
 */
void printSummary(ProfileInformation *pInformation);

/**
 * Print the header of the program
 */
void printHelp();

/**
 * Print the header of the program
 */
void printHeader();

/**
 * Run openmp test code
 */
void openmpTestCode(void);

#endif// MULTIPROCOPENCL_UTIL_H
