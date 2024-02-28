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

void runZnccFlowForOneImage(const char * imagePath, const char * outputPath) {
    Image *im = loadImage(imagePath);
    Image *smallImage = imageScaleDown(im);
    Image* grayIm = getGrayScaleImage(smallImage);
    saveImage(outputPath, grayIm);

    freeImage(im);
    freeImage(grayIm);
    freeImage(smallImage);
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
            printf("Running ZNCC flow iteration : %d,\t time : ", i + 1);
            GET_TIME(t0);
            runZnccFlowForOneImage(imagePath, outputPath);
            GET_TIME(t1);
            float elapsed_time = elapsed_time_microsec(&t0, &t1, &sec, &nsec);
            printf("\t%f micro seconds\n", elapsed_time);
            times_2[i] = elapsed_time;
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


void createSampleTestPng() {
    const char* filename = "test.png";

    /*generate some image*/
    unsigned width = 512, height = 512;
    unsigned char* image = malloc(width * height * 4);
    unsigned x, y;
    for(y = 0; y < height; y++)
        for(x = 0; x < width; x++) {
            image[4 * width * y + 4 * x + 0] = 255 * !(x & y); // red
            image[4 * width * y + 4 * x + 1] = x ^ y;       // green
            image[4 * width * y + 4 * x + 2] = x | y;       // blue
            image[4 * width * y + 4 * x + 3] = 255;         // alpha
        }

    /*run an example*/
    encodeOneStep(filename, image, width, height);

    free(image);
}