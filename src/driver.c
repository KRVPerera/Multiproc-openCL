#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <util.h>
#include <pngloader.h>
#include "config.h"
#include <cross_checking.h>
#include <zncc_c_imp.h>
#include <occlusion_filling.h>

// TODO: since the filter is syymetrical we may want to keep only wanted values
// TODO: we may want to use x and y componets of the filter separately
unsigned char* getGaussianFilter() {
    unsigned char *filter = malloc(25 * sizeof(unsigned char));
    filter[0]   = 1; filter[1]  = 4;    filter[2]   = 7;    filter[3]   = 4;    filter[4]   = 1;
    filter[5]   = 4; filter[6]  = 16;   filter[7]   = 26;   filter[8]   = 16;   filter[9]   = 4;
    filter[10]  = 7; filter[11] = 26;   filter[12]  = 41;   filter[13]  = 26;   filter[14]  = 7;
    filter[15]  = 4; filter[16] = 16;   filter[17]  = 26;   filter[18]  = 16;   filter[19]  = 4;
    filter[20]  = 1; filter[21] = 4;    filter[22]  = 7;    filter[23]  = 4;    filter[24]  = 1;
    return filter;
}

unsigned char* getMeanFilter() {
    unsigned char *filter = malloc(25 * sizeof(unsigned char));
    for (int i = 0; i < 25; ++i) {
        filter[i] = 1;
    }
    return filter;
}

// open text file to write line by line
FILE* openfile(const char *filename) {
    FILE *fp = fopen(filename, "w");

    if (fp == NULL) {
        printf("Error opening file! : %s\n", filename);
    }
    return fp;
}

void closefile(FILE *fp) {
    fclose(fp);
}


Image* getBWImage(const char * imagePath, const char * outputPath, const char * profilePath) {
    struct timespec t0, t1;
    unsigned long sec, nsec;
    FILE *fp = openfile(profilePath);

    GET_TIME(t0);
    Image *im = readImage(imagePath);
    GET_TIME(t1);
    float elapsed_time = elapsed_time_microsec(&t0, &t1, &sec, &nsec);
    fprintf(fp, "Image Load Time : %f micro seconds\n", elapsed_time);

    GET_TIME(t0);
    Image *smallImage = resizeImage(im);
    GET_TIME(t1);
    elapsed_time = elapsed_time_microsec(&t0, &t1, &sec, &nsec);
    fprintf(fp, "Image Resize Time : %f micro seconds\n", elapsed_time);

    GET_TIME(t0);
    Image* grayIm = grayScaleImage(smallImage);
    GET_TIME(t1);
    elapsed_time = elapsed_time_microsec(&t0, &t1, &sec, &nsec);
    fprintf(fp, "Image GrayScale Time : %f micro seconds\n", elapsed_time);

    unsigned char* gaussianFilter = getGaussianFilter();
    GET_TIME(t0);
    Image* filteredImage = applyFilter(grayIm, gaussianFilter, 273, 5);
    GET_TIME(t1);
    elapsed_time = elapsed_time_microsec(&t0, &t1, &sec, &nsec);
    fprintf(fp, "Image Filter Time : %f micro seconds\n", elapsed_time);

    saveImage(OUTPUT_FILE_0_BW_FILTERED, filteredImage);

    GET_TIME(t0);
    saveImage(outputPath, grayIm);
    GET_TIME(t1);
    elapsed_time = elapsed_time_microsec(&t0, &t1, &sec, &nsec);
    fprintf(fp, "Image Save Time : %f micro seconds\n", elapsed_time);

    freeImage(im);
    freeImage(smallImage);
    free(gaussianFilter);
    free(filteredImage);

    closefile(fp);
    return grayIm;
}

void postProcessFlow() {
    struct timespec t0, t1;
    unsigned long sec, nsec;
    Image* bwImage0 = readImage(OUTPUT_FILE_LEFT_DISPARITY);
    Image* bwImage1 = readImage(OUTPUT_FILE_RIGHT_DISPARITY);

    GET_TIME(t0);
    Image* crossCheckLeft = CrossCheck(bwImage0, bwImage1, CROSS_CHECKING_THRESHOLD);
    GET_TIME(t1);
    float elapsed_time = elapsed_time_microsec(&t0, &t1, &sec, &nsec);
    printf("Cross Check Time Left : %f micro seconds\n", elapsed_time);

    GET_TIME(t0);
    Image* occlusionFilledLeft = OcclusionFill(crossCheckLeft);
    GET_TIME(t1);
    elapsed_time = elapsed_time_microsec(&t0, &t1, &sec, &nsec);
    printf("Occlusion Fill Time Left : %f micro seconds\n", elapsed_time);

    saveImage(OUTPUT_FILE_OCCULSION_FILLED_LEFT, occlusionFilledLeft);

    saveImage(OUTPUT_FILE_CROSS_CHECKING_LEFT, crossCheckLeft);

    freeImage(bwImage0);
    freeImage(bwImage1);
    freeImage(crossCheckLeft);
    freeImage(occlusionFilledLeft);
}


void fullFlow() {
    struct timespec t0, t1;
    unsigned long sec, nsec;

    Image* bwImage0 = getBWImage(INPUT_FILE_0, OUTPUT_FILE_0_BW, OUTPUT_FILE_PROFILE_FILTERED_0);
    Image* bwImage1= getBWImage(INPUT_FILE_1, OUTPUT_FILE_1_BW, OUTPUT_FILE_PROFILE_FILTERED_1);

    GET_TIME(t0);
    Image* left_disparity_image = Get_zncc_c_imp(bwImage0, bwImage1, 1);
    GET_TIME(t1);
    float elapsed_time = elapsed_time_microsec(&t0, &t1, &sec, &nsec);
    printf("Left Disparity Time : %f micro seconds\n", elapsed_time);

    GET_TIME(t0);
    Image* right_disparity_image = Get_zncc_c_imp(bwImage1, bwImage0, -1);
    GET_TIME(t1);
    elapsed_time = elapsed_time_microsec(&t0, &t1, &sec, &nsec);
    printf("Right Disparity Time : %f micro seconds\n", elapsed_time);

    GET_TIME(t0);
    Image* crossCheckLeft = CrossCheck(left_disparity_image, right_disparity_image, CROSS_CHECKING_THRESHOLD);
    GET_TIME(t1);
    elapsed_time = elapsed_time_microsec(&t0, &t1, &sec, &nsec);
    printf("Cross Check Time Left : %f micro seconds\n", elapsed_time);

    GET_TIME(t0);
    Image* crossCheckRight = CrossCheck(right_disparity_image, left_disparity_image, CROSS_CHECKING_THRESHOLD);
    GET_TIME(t1);
    elapsed_time = elapsed_time_microsec(&t0, &t1, &sec, &nsec);
    printf("Cross Check Time Right : %f micro seconds\n", elapsed_time);

    GET_TIME(t0);
    Image* occlusionFilledLeft = OcclusionFill(crossCheckLeft);
    GET_TIME(t1);
    elapsed_time = elapsed_time_microsec(&t0, &t1, &sec, &nsec);
    printf("Occlusion Fill Time Left : %f micro seconds\n", elapsed_time);

    GET_TIME(t0);
    Image* occlusionFilledRight = OcclusionFill(crossCheckRight);
    GET_TIME(t1);
    elapsed_time = elapsed_time_microsec(&t0, &t1, &sec, &nsec);
    printf("Occlusion Fill Time Right : %f micro seconds\n", elapsed_time);

    saveImage(OUTPUT_FILE_OCCULSION_FILLED_LEFT, occlusionFilledLeft);
    saveImage(OUTPUT_FILE_OCCULSION_FILLED_RIGHT, occlusionFilledRight);

    saveImage(OUTPUT_FILE_CROSS_CHECKING_LEFT, crossCheckLeft);
    saveImage(OUTPUT_FILE_CROSS_CHECKING_RIGHT, crossCheckRight);

    saveImage(OUTPUT_FILE_LEFT_DISPARITY, left_disparity_image);
    saveImage(OUTPUT_FILE_RIGHT_DISPARITY, right_disparity_image);

    freeImage(bwImage0);
    freeImage(bwImage1);
    freeImage(crossCheckLeft);
    freeImage(crossCheckRight);
    freeImage(occlusionFilledLeft);
    freeImage(occlusionFilledRight);
    freeImage(left_disparity_image);
    freeImage(right_disparity_image);
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
