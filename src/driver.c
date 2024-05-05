#include "driver.h"
#include "config.h"
#include <assert.h>
#include <cross_checking.h>
#include <logger.h>
#include <occlusion_filling.h>
#include <pngloader.h>
#include <stdio.h>
#include <stdlib.h>
#include <util.h>
#include <zncc_c_imp.h>


// TODO: since the filter is syymetrical we may want to keep only wanted values
// TODO: we may want to use x and y componets of the filter separately
unsigned char *getGaussianFilter()
{
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

unsigned char *getMeanFilter()
{
    unsigned char *filter = malloc(25 * sizeof(unsigned char));
    for (int i = 0; i < 25; ++i) { filter[i] = 1; }
    return filter;
}

Image *getBWImageSingleRuns(const char *imagePath, const char *outputPath)
{
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

void readImageDriverTimes(const char *filename, ProfileInformation *profileInformation)
{
    struct timespec t0, t1;
    unsigned long sec, nsec;

    int sampleCount = profileInformation->readImage->numSamples;
    for (int i = 0; i < sampleCount; i++) {
        float elapsed_time;
        GET_TIME(t0)
        Image *im = readImage(filename);
        GET_TIME(t1)
        elapsed_time = elapsed_time_microsec(&t0, &t1, &sec, &nsec);
        profileInformation->readImage->elapsedTimes[i] = elapsed_time;
        freeImage(im);
    }
}

void resizeImageDriverTimes(Image *inputImage, ProfileInformation *profileInformation)
{
    struct timespec t0, t1;
    unsigned long sec, nsec;

    int sampleCount = profileInformation->resizeImage->numSamples;
    for (int i = 0; i < sampleCount; i++) {
        float elapsed_time;
        GET_TIME(t0)
        Image *im = resizeImage(inputImage);
        GET_TIME(t1)
        elapsed_time = elapsed_time_microsec(&t0, &t1, &sec, &nsec);
        profileInformation->resizeImage->elapsedTimes[i] = elapsed_time;
        freeImage(im);
    }
}

void grayScaleImageDriverTimes(Image *inputImage, ProfileInformation *profileInformation)
{
    struct timespec t0, t1;
    unsigned long sec, nsec;

    int sampleCount = profileInformation->grayScaleImage->numSamples;
    for (int i = 0; i < sampleCount; i++) {
        float elapsed_time;
        GET_TIME(t0)
        Image *im = grayScaleImage(inputImage);
        GET_TIME(t1)
        elapsed_time = elapsed_time_microsec(&t0, &t1, &sec, &nsec);
        profileInformation->grayScaleImage->elapsedTimes[i] = elapsed_time;
        freeImage(im);
    }
}

void applyFilterDriverTimes(const Image *inputImage,
  const unsigned char *filter,
  const float filterDenominator,
  const int filterSize,
  ProfileInformation *profileInformation)
{
    struct timespec t0, t1;
    unsigned long sec, nsec;

    int sampleCount = profileInformation->applyFilter->numSamples;
    for (int i = 0; i < sampleCount; i++) {
        float elapsed_time;
        GET_TIME(t0)
        Image *im = applyFilter(inputImage, filter, filterDenominator, filterSize);
        GET_TIME(t1)
        elapsed_time = elapsed_time_microsec(&t0, &t1, &sec, &nsec);
        profileInformation->applyFilter->elapsedTimes[i] = elapsed_time;
        freeImage(im);
    }
}

void saveImageTimes(const char *name, Image *inputImage, ProfileInformation *pInformation)
{
    struct timespec t0, t1;
    unsigned long sec, nsec;

    int sampleCount = pInformation->saveImage->numSamples;
    for (int i = 0; i < sampleCount; i++) {
        float elapsed_time;
        GET_TIME(t0)
        saveImage(name, inputImage);
        GET_TIME(t1)
        elapsed_time = elapsed_time_microsec(&t0, &t1, &sec, &nsec);
        pInformation->saveImage->elapsedTimes[i] = elapsed_time;
    }
}

void CrossCheckDriverTimes(Image *pImage, Image *pImage1, int threshold, ProfileInformation *pInformation)
{
    struct timespec t0, t1;
    unsigned long sec, nsec;

    int sampleCount = pInformation->crossCheck->numSamples;
    for (int i = 0; i < sampleCount; i++) {
        float elapsed_time;
        GET_TIME(t0)
        Image *im = CrossCheck(pImage, pImage1, threshold);
        GET_TIME(t1)
        elapsed_time = elapsed_time_microsec(&t0, &t1, &sec, &nsec);
        pInformation->crossCheck->elapsedTimes[i] = elapsed_time;
        freeImage(im);
    }
}

void znccCImpDriverTimes(Image *imageLeft, Image *imageRight, int direction, ProfileInformation *pInformation)
{
    struct timespec t0, t1;
    unsigned long sec, nsec;

    int sampleCount;
    if (direction == 1) {
        sampleCount = pInformation->zncc_left->numSamples;
    } else {
        sampleCount = pInformation->zncc_right->numSamples;
    }

    logger("Running `ZNCC algorithm` : %d times", sampleCount);

    for (int i = 0; i < sampleCount; i++) {
        float elapsed_time;
        GET_TIME(t0)
        Image *im = Get_zncc_c_imp(imageLeft, imageRight, direction);
        GET_TIME(t1)
        elapsed_time = elapsed_time_microsec(&t0, &t1, &sec, &nsec);
        if (direction == 1) {
            pInformation->zncc_left->elapsedTimes[i] = elapsed_time;
        } else {
            pInformation->zncc_right->elapsedTimes[i] = elapsed_time;
        }
        freeImage(im);
    }
}

/**
 * Profile information for the image processing
 *
 * @param filename
 * @param benchmark
 * @param profileInformation
 * @return
 */
Image *readImageDriver(const char *filename, BENCHMARK_MODE benchmark, ProfileInformation *pInformation)
{
    if (!benchmark) {
        logger("Running `readImage`");
        return readImage(filename);
    }
    assert(pInformation != NULL);

    // run ones with default sample size
    readImageDriverTimes(filename, pInformation);

    // check integrity of the average elapsed time if not increase sample size
    // and keep running until the average is correct
    if (!checkTimes(pInformation->readImage)) {
        readImageDriverTimes(filename, pInformation);
    }
    logger("Image Load Time \t: %.3f ms", pInformation->readImage->averageElapsedTime);

    // read image one more time to return the image
    return readImage(filename);
}

Image *resizeImageDriver(Image *inputImage, BENCHMARK_MODE benchmarkMode, ProfileInformation *pInformation)
{
    if (benchmarkMode == DO_NOT_BENCHMARK) {
        logger("Running `resizeImage`");
        return resizeImage(inputImage);
    }
    assert(pInformation != NULL);

    resizeImageDriverTimes(inputImage, pInformation);

    if (!checkTimes(pInformation->resizeImage)) {
        resizeImageDriverTimes(inputImage, pInformation);
    }

    logger("Image Resize Time \t: %.3f ms", pInformation->resizeImage->averageElapsedTime);

    // run the function one more time to return the image
    return resizeImage(inputImage);
}

Image *grayScaleImageDriver(Image *inputImage, BENCHMARK_MODE benchmarkMode, ProfileInformation *pInformation)
{
    if (benchmarkMode == DO_NOT_BENCHMARK) {
        logger("Running `grayScaleImage`");
        return grayScaleImage(inputImage);
    }
    assert(pInformation != NULL);

    grayScaleImageDriverTimes(inputImage, pInformation);

    if (!checkTimes(pInformation->grayScaleImage)) {
        grayScaleImageDriverTimes(inputImage, pInformation);
    }

    logger("Image Grayscale Time \t: %.3f ms", pInformation->grayScaleImage->averageElapsedTime);

    // run the function one more time to return the image
    return grayScaleImage(inputImage);
}


void saveImageDriver(const char *fileName, Image *inputImage, BENCHMARK_MODE benchmarkMode, ProfileInformation *pInformation)
{
    if (benchmarkMode == DO_NOT_BENCHMARK) {
        logger("Running `saveImage`");
        saveImage(fileName, inputImage);
    }
    assert(pInformation != NULL);

    saveImageTimes(fileName, inputImage, pInformation);

    if (!checkTimes(pInformation->saveImage)) {
        saveImageTimes(fileName, inputImage, pInformation);
    }

    logger("Image SaveImage Time \t: %.3f ms", pInformation->saveImage->averageElapsedTime);

    // run the function one more time to return the image
    saveImage(fileName, inputImage);
}

Image *CrossCheckDriver(Image *pImage, Image *pImage1, int cross_check_threshold, BENCHMARK_MODE benchmarkMode, ProfileInformation *pInformation)
{
    if (benchmarkMode == DO_NOT_BENCHMARK) {
        logger("Running `CrossCheck`");
        return CrossCheck(pImage, pImage1, cross_check_threshold);
    }
    assert(pInformation != NULL);

    CrossCheckDriverTimes(pImage, pImage1, cross_check_threshold, pInformation);

    if (!checkTimes(pInformation->crossCheck)) {
        CrossCheckDriverTimes(pImage, pImage1, cross_check_threshold, pInformation);
    }

    logger("Cross Check Time \t: %.3f ms\n", pInformation->crossCheck->averageElapsedTime);

    // run the function one more time to return the image
    return CrossCheck(pImage, pImage1, cross_check_threshold);
}

Image *applyFilterDriver(const Image *inputImage,
  const unsigned char *filter,
  const float filterDenominator,
  const int filterSize,
  BENCHMARK_MODE benchmarkMode,
  ProfileInformation *pInformation)
{
    if (benchmarkMode == DO_NOT_BENCHMARK) {
        logger("Running `applyFilter`");
        return applyFilter(inputImage, filter, filterDenominator, filterSize);
    }
    assert(pInformation != NULL);

    applyFilterDriverTimes(inputImage, filter, filterDenominator, filterSize, pInformation);

    if (!checkTimes(pInformation->applyFilter)) {
        applyFilterDriverTimes(inputImage, filter, filterDenominator, filterSize, pInformation);
    }

    logger("Image applyFilter Time : %.3f ms", pInformation->applyFilter->averageElapsedTime);

    // run the function one more time to return the image
    return applyFilter(inputImage, filter, filterDenominator, filterSize);
}

Image *znccCImpDriver(Image *pImage, Image *pImage1, int direction, BENCHMARK_MODE benchmark, ProfileInformation *pInformation)
{
    if (benchmark == DO_NOT_BENCHMARK) {
        logger("Running `Get_zncc_c_imp`");
        return Get_zncc_c_imp(pImage, pImage1, direction);
    }
    assert(pInformation != NULL);
    logger("Running `ZNCC algorithm` in benchmark mode. Please wait...");
    reinitProcessTime(pInformation->zncc_left, 3);
    znccCImpDriverTimes(pImage, pImage1, direction, pInformation);

    if (direction == 1 && !checkTimes(pInformation->zncc_left)) {
        znccCImpDriverTimes(pImage, pImage1, direction, pInformation);
    } else if (direction == -1 && !checkTimes(pInformation->zncc_right)) {
        znccCImpDriverTimes(pImage, pImage1, direction, pInformation);
    }

    if (direction == 1) {
        logger("Left Disparity Time \t: %.3f ms", pInformation->zncc_left->averageElapsedTime);
    } else {
        logger("Right Disparity Time \t: %.3f ms", pInformation->zncc_right->averageElapsedTime);
    }

    // run the function one more time to return the image
    return Get_zncc_c_imp(pImage, pImage1, direction);
}

Image *getBWImage(const char *imagePath, const char *outputPath, BENCHMARK_MODE benchmarking, ProfileInformation *profileInformation)
{
    if (benchmarking == DO_NOT_BENCHMARK) {
        assert(profileInformation == NULL);
        return getBWImageSingleRuns(imagePath, outputPath);
    }

    assert(profileInformation != NULL);

    Image *im = readImageDriver(imagePath, benchmarking, profileInformation);

    Image *smallImage = resizeImageDriver(im, benchmarking, profileInformation);

    Image *grayIm = grayScaleImageDriver(smallImage, benchmarking, profileInformation);

    unsigned char *gaussianFilter = getGaussianFilter();
    Image *filteredImage = applyFilterDriver(grayIm, gaussianFilter, 273, 5, benchmarking, profileInformation);

    saveImageDriver(OUTPUT_FILE_0_BW_FILTERED, filteredImage, benchmarking, profileInformation);
    saveImageDriver(outputPath, grayIm, 0, profileInformation);

    freeImage(im);
    freeImage(smallImage);
    free(gaussianFilter);
    free(filteredImage);

    return grayIm;
}


Image *getBWImage_MT(const char *imagePath, const char *outputPath)
{
    struct timespec t0, t1;
    unsigned long sec, nsec;

    GET_TIME(t0)
    Image *im = readImage(imagePath);
    GET_TIME(t1)
    float elapsed_time = elapsed_time_microsec(&t0, &t1, &sec, &nsec);
    logger("Image Load Time : %f micro seconds\n", elapsed_time);

    GET_TIME(t0);
    Image *smallImage = resizeImage_MT(im);
    GET_TIME(t1);
    elapsed_time = elapsed_time_microsec(&t0, &t1, &sec, &nsec);
    logger("Image Resize Time MT : %f micro seconds\n", elapsed_time);
    freeImage(im);

    GET_TIME(t0);
    Image *grayIm = grayScaleImage_MT(smallImage);
    GET_TIME(t1);
    elapsed_time = elapsed_time_microsec(&t0, &t1, &sec, &nsec);
    logger("Image GrayScale Time MT : %f micro seconds\n", elapsed_time);
    freeImage(smallImage);

    unsigned char *gaussianFilter = getGaussianFilter();
    GET_TIME(t0);
    Image *filteredImage = applyFilter_MT(grayIm, gaussianFilter, 273, 5);
    GET_TIME(t1);
    elapsed_time = elapsed_time_microsec(&t0, &t1, &sec, &nsec);
    logger("Image Filter Time : %f micro seconds\n", elapsed_time);
    saveImage(OUTPUT_FILE_0_BW_FILTERED, filteredImage);
    free(filteredImage);

    GET_TIME(t0);
    saveImage(outputPath, grayIm);
    GET_TIME(t1);
    elapsed_time = elapsed_time_microsec(&t0, &t1, &sec, &nsec);
    logger("Image Save Time : %f micro seconds\n", elapsed_time);

    free(gaussianFilter);

    return grayIm;
}

void postProcessFlow()
{
    struct timespec t0, t1;
    unsigned long sec, nsec;
    Image *bwImage0 = readImage(OUTPUT_FILE_LEFT_DISPARITY);
    Image *bwImage1 = readImage(OUTPUT_FILE_RIGHT_DISPARITY);

    GET_TIME(t0);
    Image *crossCheckLeft = CrossCheck(bwImage0, bwImage1, CROSS_CHECKING_THRESHOLD);
    GET_TIME(t1);
    float elapsed_time = elapsed_time_microsec(&t0, &t1, &sec, &nsec);
    logger("Cross Check Time Left : %f micro seconds\n", elapsed_time);

    GET_TIME(t0);
    Image *occlusionFilledLeft = OcclusionFill(crossCheckLeft);
    GET_TIME(t1);
    elapsed_time = elapsed_time_microsec(&t0, &t1, &sec, &nsec);
    logger("Occlusion Fill Time Left : %f micro seconds\n", elapsed_time);

    saveImage(OUTPUT_FILE_OCCULSION_FILLED_LEFT, occlusionFilledLeft);

    saveImage(OUTPUT_FILE_CROSS_CHECKING_LEFT, crossCheckLeft);

    freeImage(bwImage0);
    freeImage(bwImage1);
    freeImage(crossCheckLeft);
    freeImage(occlusionFilledLeft);
}

void fullFlow(BENCHMARK_MODE benchmarking)
{
    ProfileInformation *profileInformation = NULL;
    if (benchmarking == BENCHMARK) {
        logger("Running in benchmark mode");
        logger("Please note that each algorithm will be run atleast 10 times");
        logger("Average time will be reported with 95 percent confidence");
        profileInformation = createProfileInformation(10);
    }
    struct timespec t0, t1;
    unsigned long sec, nsec;

    // profileing done only on first gray scale image generation
    Image *bwImage0 = getBWImage(INPUT_FILE_0, OUTPUT_FILE_0_BW, benchmarking, profileInformation);
    Image *bwImage1 = getBWImage(INPUT_FILE_1, OUTPUT_FILE_1_BW, DO_NOT_BENCHMARK, NULL);

    // profileing done only on first disparity image generation
    Image *left_disparity_image = znccCImpDriver(bwImage0, bwImage1, 1, benchmarking, profileInformation);
    Image *right_disparity_image = znccCImpDriver(bwImage1, bwImage0, -1, DO_NOT_BENCHMARK, NULL);

    // profileing done only on first cross check image generation
    Image *crossCheckLeft = CrossCheckDriver(left_disparity_image, right_disparity_image, CROSS_CHECKING_THRESHOLD, benchmarking, profileInformation);
    Image *crossCheckRight = CrossCheckDriver(right_disparity_image, left_disparity_image, CROSS_CHECKING_THRESHOLD, DO_NOT_BENCHMARK, NULL);

    float elapsed_time_2 = elapsed_time_microsec(&t0, &t1, &sec, &nsec);
    logger("Cross Check Time Right : %f micro seconds\n", elapsed_time_2);

    GET_TIME(t0);
    Image *occlusionFilledLeft = OcclusionFill(crossCheckLeft);
    GET_TIME(t1);
    float elapsed_time_1 = elapsed_time_microsec(&t0, &t1, &sec, &nsec);
    logger("Occlusion Fill Time Left : %f micro seconds\n", elapsed_time_2);

    GET_TIME(t0);
    Image *occlusionFilledRight = OcclusionFill(crossCheckRight);
    GET_TIME(t1);
    elapsed_time_2 = elapsed_time_microsec(&t0, &t1, &sec, &nsec);
    logger("Occlusion Fill Time Right : %f micro seconds\n", elapsed_time_2);

    // average occlusion fill time
    float avg_occlusion_fill_time = (elapsed_time_1 + elapsed_time_2) / 2;
    logger("Average Occlusion Fill Time : %f micro seconds\n", avg_occlusion_fill_time);

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
    freeProfileInformation(profileInformation);
}

void fullFlow_MT()
{
    struct timespec t0, t1;
    unsigned long sec, nsec;

    Image *bwImage0 = getBWImage_MT(INPUT_FILE_0, OUTPUT_FILE_0_BW);
    Image *bwImage1 = getBWImage_MT(INPUT_FILE_1, OUTPUT_FILE_1_BW);

    GET_TIME(t0);
    Image *left_disparity_image = Get_zncc_c_imp_MT(bwImage0, bwImage1, 1);
    GET_TIME(t1);
    float elapsed_time_1 = elapsed_time_microsec(&t0, &t1, &sec, &nsec);
    logger("Left Disparity Time MT : %f micro seconds\n", elapsed_time_1);
    saveImage(OUTPUT_FILE_LEFT_DISPARITY_MT, left_disparity_image);

    GET_TIME(t0);
    Image *right_disparity_image = Get_zncc_c_imp_MT(bwImage1, bwImage0, -1);
    GET_TIME(t1);
    float elapsed_time_2 = elapsed_time_microsec(&t0, &t1, &sec, &nsec);
    logger("Right Disparity Time MT : %f micro seconds\n", elapsed_time_2);
    saveImage(OUTPUT_FILE_RIGHT_DISPARITY_MT, right_disparity_image);
    freeImage(bwImage1);
    freeImage(bwImage0);

    // average disparity time
    float avg_disparity_time = (elapsed_time_1 + elapsed_time_2) / 2;
    logger("Average Disparity Time MT : %f micro seconds\n", avg_disparity_time);

    GET_TIME(t0);
    Image *crossCheckLeft = CrossCheck_MT(left_disparity_image, right_disparity_image, CROSS_CHECKING_THRESHOLD);
    GET_TIME(t1);
    elapsed_time_1 = elapsed_time_microsec(&t0, &t1, &sec, &nsec);
    logger("Cross Check Time Left MT : %f micro seconds\n", elapsed_time_1);
    saveImage(OUTPUT_FILE_CROSS_CHECKING_LEFT_MT, crossCheckLeft);

    GET_TIME(t0);
    Image *crossCheckRight = CrossCheck_MT(right_disparity_image, left_disparity_image, CROSS_CHECKING_THRESHOLD);
    GET_TIME(t1);
    elapsed_time_2 = elapsed_time_microsec(&t0, &t1, &sec, &nsec);
    logger("Cross Check Time Right MT : %f micro seconds\n", elapsed_time_2);
    saveImage(OUTPUT_FILE_CROSS_CHECKING_RIGHT_MT, crossCheckRight);
    freeImage(right_disparity_image);
    freeImage(left_disparity_image);

    // average cross check time
    float avg_cross_check_time = (elapsed_time_1 + elapsed_time_2) / 2;
    logger("Average Cross Check Time MT : %f micro seconds\n", avg_cross_check_time);

    GET_TIME(t0);
    Image *occlusionFilledLeft = OcclusionFill_MT(crossCheckLeft);
    GET_TIME(t1);
    elapsed_time_1 = elapsed_time_microsec(&t0, &t1, &sec, &nsec);
    logger("Occlusion Fill Time Left MT : %f micro seconds\n", elapsed_time_1);
    saveImage(OUTPUT_FILE_OCCULSION_FILLED_LEFT_MT, occlusionFilledLeft);
    freeImage(occlusionFilledLeft);
    freeImage(crossCheckLeft);

    GET_TIME(t0);
    Image *occlusionFilledRight = OcclusionFill_MT(crossCheckRight);
    GET_TIME(t1);
    elapsed_time_2 = elapsed_time_microsec(&t0, &t1, &sec, &nsec);
    logger("Occlusion Fill Time Right MT : %f micro seconds\n", elapsed_time_2);
    saveImage(OUTPUT_FILE_OCCULSION_FILLED_RIGHT_MT, occlusionFilledRight);
    freeImage(occlusionFilledRight);
    freeImage(crossCheckRight);

    // average occlusion fill time
    float avg_occlusion_fill_time = (elapsed_time_1 + elapsed_time_2) / 2;
    logger("Average Occlusion Fill Time MT : %f micro seconds\n", avg_occlusion_fill_time);
}

void runZnccFlowForOneImage(const char *imagePath, const char *outputPath)
{
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

void zncc_flow_driver(const char *imagePath, const char *outputPath)
{
    struct timespec t0, t1;
    unsigned long sec, nsec;
    float mean, sd;
    int req_n;
    float times[10];
    logger("Running ZNCC flow for 10 times\n");
    for (int i = 0; i < 10; ++i) {
        logger("Running ZNCC flow iteration : %d,\t time : ", i + 1);
        GET_TIME(t0);
        runZnccFlowForOneImage(imagePath, outputPath);
        GET_TIME(t1);
        float elapsed_time = elapsed_time_microsec(&t0, &t1, &sec, &nsec);
        logger("\t%f micro seconds\n", elapsed_time);
        times[i] = elapsed_time;
    }
    mean = Average(times, 10);
    sd = standardDeviation(times, 10);
    req_n = requiredSampleSize(sd, mean);
    logger("Average time : %f micro seconds\n", mean);
    logger("Required sample size for 95 percent confidence with 5 percent error margin : %d\n", req_n);

    if (req_n > 10) {
        logger("Running ZNCC flow for %d times\n", req_n);
        float *times_2 = (float *)malloc(sizeof(float) * req_n);
        for (int i = 0; i < req_n; ++i) {
            GET_TIME(t0);
            runZnccFlowForOneImage(imagePath, outputPath);
            GET_TIME(t1);
            times_2[i] = elapsed_time_microsec(&t0, &t1, &sec, &nsec);
        }
        mean = Average(times_2, req_n);
        sd = standardDeviation(times_2, req_n);
        req_n = requiredSampleSize(sd, mean);
        logger("Average time : %f micro seconds\n", mean);
        logger("Required sample size for 95 percent confidence with 5 percent error margin : %d\n", req_n);
    } else {
        logger("Sample size is less than 10, so the confidence interval may be accurate\n");
    }
}
