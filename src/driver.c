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
unsigned char *getGaussianFilter() {
    unsigned char *filter = malloc(25 * sizeof(unsigned char));
    filter[0] = 1;
    filter[1] = 4;
    filter[2] = 7;
    filter[3] = 4;
    filter[4] = 1;
    filter[5] = 4;
    filter[6] = 16;
    filter[7] = 26;
    filter[8] = 16;
    filter[9] = 4;
    filter[10] = 7;
    filter[11] = 26;
    filter[12] = 41;
    filter[13] = 26;
    filter[14] = 7;
    filter[15] = 4;
    filter[16] = 16;
    filter[17] = 26;
    filter[18] = 16;
    filter[19] = 4;
    filter[20] = 1;
    filter[21] = 4;
    filter[22] = 7;
    filter[23] = 4;
    filter[24] = 1;
    return filter;
}

unsigned char *getMeanFilter() {
    unsigned char *filter = malloc(25 * sizeof(unsigned char));
    for (int i = 0; i < 25; ++i) {
        filter[i] = 1;
    }
    return filter;
}

// open text file to write line by line
FILE *openfile(const char *filename) {
    FILE *fp = fopen(filename, "w");

    if (fp == NULL) {
        printf("Error opening file! : %s\n", filename);
    }
    return fp;
}

void closefile(FILE *fp) {
    fclose(fp);
}

Image *getBWImageSingleRuns(const char *imagePath, const char *outputPath) {
    Image *im = readImage(imagePath);

    Image *smallImage = resizeImage(im);

    Image *grayIm = grayScaleImage(smallImage);

    unsigned char *gaussianFilter = getGaussianFilter();
    Image *filteredImage = applyFilter(grayIm, gaussianFilter, 273, 5);

    saveImage(OUTPUT_FILE_0_BW_FILTERED, filteredImage);

    saveImage(outputPath, grayIm);

    freeImage(im);
    freeImage(smallImage);
    free(gaussianFilter);
    free(filteredImage);

    return grayIm;
}

Image *getBWImage(const char *imagePath, const char *outputPath, int benchmarking) {
    struct timespec t0, t1;
    unsigned long sec, nsec;

    float elapsed_time;
    Image *im;

    if (benchmarking) {
        // first try running readImage 10 times to get average time
        int numberOfSamples = 10;
        // allocate memory for 10 samples
        float *times = (float *) malloc(sizeof(float) * 10);
        int isAverageOkay = 0;
        printf("Running readImage function for %d times\n", numberOfSamples);
        while (!isAverageOkay) {
            for (int i = 0; i < numberOfSamples; i++) {
                GET_TIME(t0)
                im = readImage(imagePath);
                GET_TIME(t1)
                elapsed_time = elapsed_time_microsec(&t0, &t1, &sec, &nsec);
                times[i] = elapsed_time;
                if (i < numberOfSamples - 1) {
                    freeImage(im);
                }
            }
            // get the average time for `readImage` function
            float mean = Average(times, numberOfSamples);
            float sd = standardDeviation(times, numberOfSamples);

            // get required sample size for 95% confidence with 5% error margin
            int req_n = requiredSampleSize(sd, mean);
            // if required sample size is greater than 10, then run the function again with desired sample size
            if (req_n > 10) {
                printf("Required sample size for 95 percent confidence with 5 percent error margin : %d\n", req_n);
                printf("Running readImage function for %d times\n", req_n);
                numberOfSamples = req_n;
                free(times);
                freeImage(im);
                times = (float *) malloc(sizeof(float) * numberOfSamples);
            } else {
                isAverageOkay = 1;
                elapsed_time = mean;
            }
        }
        free(times);
        printf("Image Load Time : %f micro seconds\n", elapsed_time);
    } else {
        getBWImageSingleRuns(imagePath, outputPath);
    }

    GET_TIME(t0)
    Image *smallImage = resizeImage(im);
    GET_TIME(t1)
    elapsed_time = elapsed_time_microsec(&t0, &t1, &sec, &nsec);
    printf("Image Resize Time : %f micro seconds\n", elapsed_time);

    GET_TIME(t0)
    Image *grayIm = grayScaleImage(smallImage);
    GET_TIME(t1)
    elapsed_time = elapsed_time_microsec(&t0, &t1, &sec, &nsec);
    printf("Image GrayScale Time : %f micro seconds\n", elapsed_time);

    unsigned char *gaussianFilter = getGaussianFilter();
    GET_TIME(t0)
    Image *filteredImage = applyFilter(grayIm, gaussianFilter, 273, 5);
    GET_TIME(t1)
    elapsed_time = elapsed_time_microsec(&t0, &t1, &sec, &nsec);
    printf("Image Filter Time : %f micro seconds\n", elapsed_time);

    saveImage(OUTPUT_FILE_0_BW_FILTERED, filteredImage);

    GET_TIME(t0)
    saveImage(outputPath, grayIm);
    GET_TIME(t1)
    elapsed_time = elapsed_time_microsec(&t0, &t1, &sec, &nsec);
    printf("Image Save Time : %f micro seconds\n", elapsed_time);

    freeImage(im);
    freeImage(smallImage);
    free(gaussianFilter);
    free(filteredImage);

    return grayIm;
}

Image *getBWImage_MT(const char *imagePath, const char *outputPath, const char *profilePath) {
    struct timespec t0, t1;
    unsigned long sec, nsec;
    FILE *fp = openfile(profilePath);

    GET_TIME(t0);
    Image *im = readImage(imagePath);
    GET_TIME(t1);
    float elapsed_time = elapsed_time_microsec(&t0, &t1, &sec, &nsec);
    fprintf(fp, "Image Load Time : %f micro seconds\n", elapsed_time);

    GET_TIME(t0);
    Image *smallImage = resizeImage_MT(im);
    GET_TIME(t1);
    elapsed_time = elapsed_time_microsec(&t0, &t1, &sec, &nsec);
    fprintf(fp, "Image Resize Time MT : %f micro seconds\n", elapsed_time);
    freeImage(im);

    GET_TIME(t0);
    Image *grayIm = grayScaleImage_MT(smallImage);
    GET_TIME(t1);
    elapsed_time = elapsed_time_microsec(&t0, &t1, &sec, &nsec);
    fprintf(fp, "Image GrayScale Time MT : %f micro seconds\n", elapsed_time);
    freeImage(smallImage);

    unsigned char *gaussianFilter = getGaussianFilter();
    GET_TIME(t0);
    Image *filteredImage = applyFilter_MT(grayIm, gaussianFilter, 273, 5);
    GET_TIME(t1);
    elapsed_time = elapsed_time_microsec(&t0, &t1, &sec, &nsec);
    fprintf(fp, "Image Filter Time : %f micro seconds\n", elapsed_time);
    saveImage(OUTPUT_FILE_0_BW_FILTERED, filteredImage);
    free(filteredImage);

    GET_TIME(t0);
    saveImage(outputPath, grayIm);
    GET_TIME(t1);
    elapsed_time = elapsed_time_microsec(&t0, &t1, &sec, &nsec);
    fprintf(fp, "Image Save Time : %f micro seconds\n", elapsed_time);

    free(gaussianFilter);

    closefile(fp);
    return grayIm;
}

void postProcessFlow() {
    struct timespec t0, t1;
    unsigned long sec, nsec;
    Image *bwImage0 = readImage(OUTPUT_FILE_LEFT_DISPARITY);
    Image *bwImage1 = readImage(OUTPUT_FILE_RIGHT_DISPARITY);

    GET_TIME(t0);
    Image *crossCheckLeft = CrossCheck(bwImage0, bwImage1, CROSS_CHECKING_THRESHOLD);
    GET_TIME(t1);
    float elapsed_time = elapsed_time_microsec(&t0, &t1, &sec, &nsec);
    printf("Cross Check Time Left : %f micro seconds\n", elapsed_time);

    GET_TIME(t0);
    Image *occlusionFilledLeft = OcclusionFill(crossCheckLeft);
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


void fullFlow(int benchmarking) {
    struct timespec t0, t1;
    unsigned long sec, nsec;

    Image *bwImage0 = getBWImage(INPUT_FILE_0, OUTPUT_FILE_0_BW, benchmarking);
    Image *bwImage1 = getBWImage(INPUT_FILE_1, OUTPUT_FILE_1_BW, !benchmarking);

    GET_TIME(t0)
    Image *left_disparity_image = Get_zncc_c_imp(bwImage0, bwImage1, 1);
    GET_TIME(t1)
    float elapsed_time_1 = elapsed_time_microsec(&t0, &t1, &sec, &nsec);
    printf("Left Disparity Time : %f micro seconds\n", elapsed_time_1);

    GET_TIME(t0)
    Image *right_disparity_image = Get_zncc_c_imp(bwImage1, bwImage0, -1);
    GET_TIME(t1)
    float elapsed_time_2 = elapsed_time_microsec(&t0, &t1, &sec, &nsec);
    printf("Right Disparity Time : %f micro seconds\n", elapsed_time_2);

    // average disparity time
    float avg_disparity_time = (elapsed_time_1 + elapsed_time_2) / 2;
    printf("Average Disparity Time : %f micro seconds\n", avg_disparity_time);

    GET_TIME(t0)
    Image *crossCheckLeft = CrossCheck(left_disparity_image, right_disparity_image, CROSS_CHECKING_THRESHOLD);
    GET_TIME(t1)
    elapsed_time_1 = elapsed_time_microsec(&t0, &t1, &sec, &nsec);
    printf("Cross Check Time Left : %f micro seconds\n", elapsed_time_1);

    GET_TIME(t0)
    Image *crossCheckRight = CrossCheck(right_disparity_image, left_disparity_image, CROSS_CHECKING_THRESHOLD);
    GET_TIME(t1)
    elapsed_time_2 = elapsed_time_microsec(&t0, &t1, &sec, &nsec);
    printf("Cross Check Time Right : %f micro seconds\n", elapsed_time_2);

    // average cross check time
    float avg_cross_check_time = (elapsed_time_1 + elapsed_time_2) / 2;
    printf("Average Cross Check Time : %f micro seconds\n", avg_cross_check_time);

    GET_TIME(t0);
    Image *occlusionFilledLeft = OcclusionFill(crossCheckLeft);
    GET_TIME(t1);
    elapsed_time_1 = elapsed_time_microsec(&t0, &t1, &sec, &nsec);
    printf("Occlusion Fill Time Left : %f micro seconds\n", elapsed_time_2);

    GET_TIME(t0);
    Image *occlusionFilledRight = OcclusionFill(crossCheckRight);
    GET_TIME(t1);
    elapsed_time_2 = elapsed_time_microsec(&t0, &t1, &sec, &nsec);
    printf("Occlusion Fill Time Right : %f micro seconds\n", elapsed_time_2);

    // average occlusion fill time
    float avg_occlusion_fill_time = (elapsed_time_1 + elapsed_time_2) / 2;
    printf("Average Occlusion Fill Time : %f micro seconds\n", avg_occlusion_fill_time);

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

void fullFlow_MT() {
    struct timespec t0, t1;
    unsigned long sec, nsec;

    Image *bwImage0 = getBWImage_MT(INPUT_FILE_0, OUTPUT_FILE_0_BW, OUTPUT_FILE_PROFILE_FILTERED_0_MT);
    Image *bwImage1 = getBWImage_MT(INPUT_FILE_1, OUTPUT_FILE_1_BW, OUTPUT_FILE_PROFILE_FILTERED_1_MT);

    GET_TIME(t0);
    Image *left_disparity_image = Get_zncc_c_imp_MT(bwImage0, bwImage1, 1);
    GET_TIME(t1);
    float elapsed_time_1 = elapsed_time_microsec(&t0, &t1, &sec, &nsec);
    printf("Left Disparity Time MT : %f micro seconds\n", elapsed_time_1);
    saveImage(OUTPUT_FILE_LEFT_DISPARITY_MT, left_disparity_image);

    GET_TIME(t0);
    Image *right_disparity_image = Get_zncc_c_imp_MT(bwImage1, bwImage0, -1);
    GET_TIME(t1);
    float elapsed_time_2 = elapsed_time_microsec(&t0, &t1, &sec, &nsec);
    printf("Right Disparity Time MT : %f micro seconds\n", elapsed_time_2);
    saveImage(OUTPUT_FILE_RIGHT_DISPARITY_MT, right_disparity_image);
    freeImage(bwImage1);
    freeImage(bwImage0);

    // average disparity time
    float avg_disparity_time = (elapsed_time_1 + elapsed_time_2) / 2;
    printf("Average Disparity Time MT : %f micro seconds\n", avg_disparity_time);

    GET_TIME(t0);
    Image *crossCheckLeft = CrossCheck_MT(left_disparity_image, right_disparity_image,
                                          CROSS_CHECKING_THRESHOLD);
    GET_TIME(t1);
    elapsed_time_1 = elapsed_time_microsec(&t0, &t1, &sec, &nsec);
    printf("Cross Check Time Left MT : %f micro seconds\n", elapsed_time_1);
    saveImage(OUTPUT_FILE_CROSS_CHECKING_LEFT_MT, crossCheckLeft);

    GET_TIME(t0);
    Image *crossCheckRight = CrossCheck_MT(right_disparity_image, left_disparity_image,
                                           CROSS_CHECKING_THRESHOLD);
    GET_TIME(t1);
    elapsed_time_2 = elapsed_time_microsec(&t0, &t1, &sec, &nsec);
    printf("Cross Check Time Right MT : %f micro seconds\n", elapsed_time_2);
    saveImage(OUTPUT_FILE_CROSS_CHECKING_RIGHT_MT, crossCheckRight);
    freeImage(right_disparity_image);
    freeImage(left_disparity_image);

    // average cross check time
    float avg_cross_check_time = (elapsed_time_1 + elapsed_time_2) / 2;
    printf("Average Cross Check Time MT : %f micro seconds\n", avg_cross_check_time);

    GET_TIME(t0);
    Image *occlusionFilledLeft = OcclusionFill_MT(crossCheckLeft);
    GET_TIME(t1);
    elapsed_time_1 = elapsed_time_microsec(&t0, &t1, &sec, &nsec);
    printf("Occlusion Fill Time Left MT : %f micro seconds\n", elapsed_time_1);
    saveImage(OUTPUT_FILE_OCCULSION_FILLED_LEFT_MT, occlusionFilledLeft);
    freeImage(occlusionFilledLeft);
    freeImage(crossCheckLeft);

    GET_TIME(t0);
    Image *occlusionFilledRight = OcclusionFill_MT(crossCheckRight);
    GET_TIME(t1);
    elapsed_time_2 = elapsed_time_microsec(&t0, &t1, &sec, &nsec);
    printf("Occlusion Fill Time Right MT : %f micro seconds\n", elapsed_time_2);
    saveImage(OUTPUT_FILE_OCCULSION_FILLED_RIGHT_MT, occlusionFilledRight);
    freeImage(occlusionFilledRight);
    freeImage(crossCheckRight);

    // average occlusion fill time
    float avg_occlusion_fill_time = (elapsed_time_1 + elapsed_time_2) / 2;
    printf("Average Occlusion Fill Time MT : %f micro seconds\n", avg_occlusion_fill_time);
}

void runZnccFlowForOneImage(const char *imagePath, const char *outputPath) {
    Image *im = readImage(imagePath);
    Image *smallImage = resizeImage(im);
    Image *grayIm = grayScaleImage(smallImage);

    unsigned char *gaussianFilter = getGaussianFilter();
    Image *filteredImage = applyFilter(grayIm, gaussianFilter, 273, 5);

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
