//
// Created by ruksh on 28/02/2024.
//
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <util.h>
#include <pngloader.h>

#define GET_TIME(x); if(clock_gettime(CLOCK_MONOTONIC, &(x)) < 0) \
{perror("clock_gettime(): "); exit(EXIT_FAILURE);}

// TODO: since the filter is syymetrical we may want to keep only wanted values
// TODO: we may want to use x and y componets of the filter separately
unsigned char* getGaussianFilter() {
    unsigned char *filter = malloc(25 * sizeof(unsigned char));
    filter[0]   = 1; filter[1]  = 4;    filter[2]   = 7;    filter[3]   = 4;    filter[4]   = 1;
    filter[5]   = 4; filter[6]  = 16;   filter[7]   = 26;   filter[8]   = 16;   filter[9]   = 4;
    filter[10]  = 7; filter[11] = 16;   filter[12]  = 41;   filter[13]  = 26;   filter[14]  = 7;
    filter[15]  = 4; filter[16] = 16;   filter[17]  = 26;   filter[18]  = 16;   filter[19]  = 4;
    filter[20]  = 1; filter[21] = 4;    filter[22]  = 7;    filter[23]  = 4;    filter[24]  = 1;
    return filter;
}


void runZnccFlowForOneImage(const char * imagePath, const char * outputPath) {
    Image *im = readImage(imagePath);
    Image *smallImage = resizeImage(im);
    Image* grayIm = grayScaleImage(smallImage);
    unsigned char* gaussianFilter = getGaussianFilter();
    Image* filteredImage = applyFilter(grayIm, gaussianFilter, 273, 5);
    saveImage(outputPath, filteredImage);

    freeImage(im);
    freeImage(grayIm);
    freeImage(smallImage);
    free(gaussianFilter);
    freeImage(filteredImage);
}

void zncc_flow_driver(const char *imagePath, const char *outputPath) {
    struct timespec t0, t1;
    unsigned long sec, nsec;
    float mean, sd;
    int req_n;
    float times[10];
    printf("Running ZNCC flow for 10 times\n");
    for (int i = 0; i < 10; ++i) {
        printf("Running ZNCC flow iteration : %d,\t time : ", i + 1);
        GET_TIME(t0);
        runZnccFlowForOneImage(imagePath, outputPath);
        GET_TIME(t1);
        float elapsed_time = elapsed_time_microsec(&t0, &t1, &sec, &nsec);
        printf("\t%f micro seconds\n", elapsed_time);
        times[i] = elapsed_time;
    }
    mean = Average(times, 10);
    sd = standardDeviation(times, 10);
    req_n = requiredSampleSize(sd, mean);
    printf("Average time : %f micro seconds\n", mean);
    printf("Required sample size for 95 percent confidence with 5 percent error margin : %d\n", req_n);

    if (req_n > 10) {
        printf("Running ZNCC flow for %d times\n", req_n);
        float *times_2 = (float *) malloc(sizeof(float) * req_n);
        for (int i = 0; i < req_n; ++i) {
            GET_TIME(t0);
            runZnccFlowForOneImage(imagePath, outputPath);
            GET_TIME(t1);
            times_2[i] = elapsed_time_microsec(&t0, &t1, &sec, &nsec);
        }
        mean = Average(times_2, req_n);
        sd = standardDeviation(times_2, req_n);
        req_n = requiredSampleSize(sd, mean);
        printf("Average time : %f micro seconds\n", mean);
        printf("Required sample size for 95 percent confidence with 5 percent error margin : %d\n", req_n);
    } else {
        printf("Sample size is less than 10, so the confidence interval may be accurate\n");
    }
}
