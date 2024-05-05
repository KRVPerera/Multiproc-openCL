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

Image *getBWImageSingleRuns_MT(const char *imagePath, const char *outputPath)
{
    Image *im = readImage(imagePath);

    Image *smallImage = resizeImage_MT(im);

    Image *grayIm = grayScaleImage_MT(smallImage);

    unsigned char *gaussianFilter = getGaussianFilter();
    Image *filteredImage = applyFilter_MT(grayIm, gaussianFilter, 273, 5);

    saveImage(OUTPUT_FILE_0_BW_FILTERED, filteredImage);

    saveImage(outputPath, grayIm);

    freeImage(im);
    freeImage(smallImage);
    free(gaussianFilter);
    free(filteredImage);

    return grayIm;
}

Image *getBWImageSingleRuns(const char *imagePath, const char *outputPath, bool multiThreaded)
{
    Image *im = readImage(imagePath);

    Image *smallImage = NULL;

    if (multiThreaded) {
        smallImage = resizeImage_MT(im);
    } else {
        smallImage = resizeImage(im);
    }

    Image *grayIm = NULL;
    if (multiThreaded) {
        grayIm = grayScaleImage_MT(smallImage);
    } else {
        grayIm = grayScaleImage(smallImage);
    }


    unsigned char *gaussianFilter = getGaussianFilter();
    Image *filteredImage = NULL;
    if (multiThreaded) {
        filteredImage = applyFilter_MT(grayIm, gaussianFilter, 273, 5);
    } else {
        filteredImage = applyFilter(grayIm, gaussianFilter, 273, 5);
    }

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

    // create a function pointer to the resizeImage function
    typedef Image *(*ResizeImageFuncPtr)(const Image *input);
    ResizeImageFuncPtr resizeImageFuncPtr = profileInformation->multiThreaded ? resizeImage_MT : resizeImage;

    int sampleCount = profileInformation->resizeImage->numSamples;
    for (int i = 0; i < sampleCount; i++) {
        float elapsed_time;
        GET_TIME(t0)
        Image *im = resizeImageFuncPtr(inputImage);
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

    // create a function pointer to the grayScaleImage function
    typedef Image *(*GrayScakeFuncPtr)(const Image *input);
    GrayScakeFuncPtr grayScaleFunctionPtr = profileInformation->multiThreaded ? grayScaleImage_MT : grayScaleImage;

    int sampleCount = profileInformation->grayScaleImage->numSamples;
    for (int i = 0; i < sampleCount; i++) {
        float elapsed_time;
        GET_TIME(t0)
        Image *im = grayScaleFunctionPtr(inputImage);
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

    // create a function pointer to the applyFilter function
    typedef Image *(*ApplyFilterFuncPtr)(const Image *input, const unsigned char *filter, float filterDenominator, int filterSize);
    ApplyFilterFuncPtr applyFilterFuncPtr = profileInformation->multiThreaded ? applyFilter_MT : applyFilter;

    int sampleCount = profileInformation->applyFilter->numSamples;
    for (int i = 0; i < sampleCount; i++) {
        float elapsed_time;
        GET_TIME(t0)
        Image *im = applyFilterFuncPtr(inputImage, filter, filterDenominator, filterSize);
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

    typedef Image *(*CrossCheckFuncPtr)(const Image *pImage, const Image *pImage1, int threshold);
    CrossCheckFuncPtr crossCheckFuncPtr = pInformation->multiThreaded ? CrossCheck_MT : CrossCheck;

    int sampleCount = pInformation->crossCheck->numSamples;
    for (int i = 0; i < sampleCount; i++) {
        float elapsed_time;
        GET_TIME(t0)
        Image *im = crossCheckFuncPtr(pImage, pImage1, threshold);
        GET_TIME(t1)
        elapsed_time = elapsed_time_microsec(&t0, &t1, &sec, &nsec);
        pInformation->crossCheck->elapsedTimes[i] = elapsed_time;
        freeImage(im);
    }
}

void OcclusionFillDriverTimes(Image *pImage, ProfileInformation *pInformation)
{
    struct timespec t0, t1;
    unsigned long sec, nsec;

    typedef Image *(*OcclusionFillFuncPtr)(const Image *pImage);
    OcclusionFillFuncPtr occlusionFillFuncPtr = pInformation->multiThreaded ? OcclusionFill_MT : OcclusionFill;

    int sampleCount = pInformation->occlusion->numSamples;
    for (int i = 0; i < sampleCount; i++) {
        float elapsed_time;
        GET_TIME(t0)
        Image *im = occlusionFillFuncPtr(pImage);
        GET_TIME(t1)
        elapsed_time = elapsed_time_microsec(&t0, &t1, &sec, &nsec);
        pInformation->occlusion->elapsedTimes[i] = elapsed_time;
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

    typedef Image *(*ZNCCFuncPtr)(const Image *pImage, const Image *pImage1, int direction);
    ZNCCFuncPtr znccFuncPtr = pInformation->multiThreaded ? Get_zncc_c_imp_MT : Get_zncc_c_imp;

    for (int i = 0; i < sampleCount; i++) {
        float elapsed_time;
        GET_TIME(t0)
        Image *im = znccFuncPtr(imageLeft, imageRight, direction);
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
        if (pInformation->multiThreaded) {
            return resizeImage_MT(inputImage);
        }
        return resizeImage(inputImage);
    }

    resizeImageDriverTimes(inputImage, pInformation);

    if (!checkTimes(pInformation->resizeImage)) {
        resizeImageDriverTimes(inputImage, pInformation);
    }

    logger("Image Resize Time \t: %.3f ms", pInformation->resizeImage->averageElapsedTime);

    // run the function one more time to return the image
    if (pInformation->multiThreaded) {
        return resizeImage_MT(inputImage);
    }
    return resizeImage(inputImage);
}

Image *grayScaleImageDriver(Image *inputImage, BENCHMARK_MODE benchmarkMode, ProfileInformation *pInformation)
{
    assert(pInformation != NULL);
    if (benchmarkMode == DO_NOT_BENCHMARK) {
        logger("Running `grayScaleImage`");
        if (pInformation->multiThreaded) {
            return grayScaleImage_MT(inputImage);
        }
        return grayScaleImage(inputImage);
    }

    grayScaleImageDriverTimes(inputImage, pInformation);

    if (!checkTimes(pInformation->grayScaleImage)) {
        grayScaleImageDriverTimes(inputImage, pInformation);
    }

    logger("Image Grayscale Time \t: %.3f ms", pInformation->grayScaleImage->averageElapsedTime);

    // run the function one more time to return the image
    if (pInformation->multiThreaded) {
        return grayScaleImage_MT(inputImage);
    }
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
        if (pInformation->multiThreaded) {
            return CrossCheck_MT(pImage, pImage1, cross_check_threshold);
        }
        return CrossCheck(pImage, pImage1, cross_check_threshold);
    }
    assert(pInformation != NULL);

    CrossCheckDriverTimes(pImage, pImage1, cross_check_threshold, pInformation);

    if (!checkTimes(pInformation->crossCheck)) {
        CrossCheckDriverTimes(pImage, pImage1, cross_check_threshold, pInformation);
    }

    logger("Cross Check Time \t: %.3f ms\n", pInformation->crossCheck->averageElapsedTime);

    // run the function one more time to return the image
    if (pInformation->multiThreaded) {
        return CrossCheck_MT(pImage, pImage1, cross_check_threshold);
    }
    return CrossCheck(pImage, pImage1, cross_check_threshold);
}

Image *OcclusionFillDriver(Image *pImage, BENCHMARK_MODE benchmarkMode, ProfileInformation *pInformation)
{

    if (benchmarkMode == DO_NOT_BENCHMARK) {
        logger("Running `OcclusionFill`");
        if (pInformation->multiThreaded) {
            return OcclusionFill_MT(pImage);
        }
        return OcclusionFill(pImage);
    }
    assert(pInformation != NULL);

    OcclusionFillDriverTimes(pImage, pInformation);

    if (!checkTimes(pInformation->occlusion)) {
        OcclusionFillDriverTimes(pImage, pInformation);
    }

    logger("Occulsion Fill Time : %.3f ms", pInformation->occlusion->averageElapsedTime);

    // run the function one more time to return the image
    if (pInformation->multiThreaded) {
        return OcclusionFill_MT(pImage);
    }
    return OcclusionFill(pImage);
}

Image *
  applyFilterDriver(const Image *inputImage, const unsigned char *filter, const float filterDenominator, const int filterSize, ProfileInformation *pInformation)
{
    applyFilterDriverTimes(inputImage, filter, filterDenominator, filterSize, pInformation);

    if (!checkTimes(pInformation->applyFilter)) {
        applyFilterDriverTimes(inputImage, filter, filterDenominator, filterSize, pInformation);
    }

    logger("Image applyFilter Time : %.3f ms", pInformation->applyFilter->averageElapsedTime);

    // run the function one more time to return the image
    if (pInformation->multiThreaded) {
        return applyFilter_MT(inputImage, filter, filterDenominator, filterSize);
    }
    return applyFilter(inputImage, filter, filterDenominator, filterSize);
}

Image *znccCImpDriver(Image *pImage, Image *pImage1, int direction, BENCHMARK_MODE benchmark, ProfileInformation *pInformation)
{
    if (benchmark == DO_NOT_BENCHMARK) {
        if (pInformation->multiThreaded) {
            logger("Running `Get_zncc_c_imp_MT`");
            return Get_zncc_c_imp_MT(pImage, pImage1, direction);
        }
        logger("Running `Get_zncc_c_imp`");
        return Get_zncc_c_imp(pImage, pImage1, direction);
    }

    logger("Running `ZNCC algorithm` in benchmark mode. Please wait...");
    if (direction == 1) {
        reinitProcessTime(&pInformation->zncc_left, 3);
        assert(pInformation->zncc_left->numSamples == 3);
    } else {
        reinitProcessTime(&pInformation->zncc_right, 3);
        assert(pInformation->zncc_right->numSamples == 3);
    }
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

    if (pInformation->multiThreaded) {
        return Get_zncc_c_imp_MT(pImage, pImage1, direction);
    }
    // run the function one more time to return the image
    return Get_zncc_c_imp(pImage, pImage1, direction);
}

Image *getBWImage(const char *imagePath, const char *outputPath, BENCHMARK_MODE benchmarking, ProfileInformation *profileInformation)
{
    if (benchmarking == DO_NOT_BENCHMARK) {
        assert(profileInformation != NULL);
        return getBWImageSingleRuns(imagePath, outputPath, profileInformation->multiThreaded);
    }

    Image *im = readImageDriver(imagePath, benchmarking, profileInformation);

    Image *smallImage = resizeImageDriver(im, benchmarking, profileInformation);

    Image *grayIm = grayScaleImageDriver(smallImage, benchmarking, profileInformation);

    unsigned char *gaussianFilter = getGaussianFilter();
    Image *filteredImage = applyFilterDriver(grayIm, gaussianFilter, 273, 5, profileInformation);

    saveImageDriver(OUTPUT_FILE_0_BW_FILTERED, filteredImage, benchmarking, profileInformation);
    saveImageDriver(outputPath, grayIm, 0, profileInformation);

    freeImage(im);
    freeImage(smallImage);
    free(gaussianFilter);
    free(filteredImage);

    return grayIm;
}

void fullFlow(BENCHMARK_MODE benchmarkMode, bool multiThreadedMode)
{
    if (multiThreadedMode) {
        logger("Running in multithreaded mode");
    }
    ProfileInformation *profileInformation = NULL;
    if (benchmarkMode == BENCHMARK) {
        logger("Running in benchmark mode");
        logger("Please note that each algorithm will be run atleast 10 times");
        logger("Average time will be reported with 95 percent confidence");
        profileInformation = createProfileInformation(10);
    } else {
        profileInformation = createProfileInformationWithoutBenchmarking();
    }
    profileInformation->multiThreaded = multiThreadedMode;

    // profiling done only on first gray scale image generation
    Image *bwImage0 = getBWImage(INPUT_FILE_0, OUTPUT_FILE_0_BW, benchmarkMode, profileInformation);
    Image *bwImage1 = getBWImage(INPUT_FILE_1, OUTPUT_FILE_1_BW, DO_NOT_BENCHMARK, profileInformation);

    // profiling done only on first disparity image generation
    Image *left_disparity_image = znccCImpDriver(bwImage0, bwImage1, 1, benchmarkMode, profileInformation);
    Image *right_disparity_image = znccCImpDriver(bwImage1, bwImage0, -1, DO_NOT_BENCHMARK, NULL);

    // profiling done only on first cross check image generation
    Image *crossCheckLeft = CrossCheckDriver(left_disparity_image, right_disparity_image, CROSS_CHECKING_THRESHOLD, benchmarkMode, profileInformation);
    Image *crossCheckRight = CrossCheckDriver(right_disparity_image, left_disparity_image, CROSS_CHECKING_THRESHOLD, DO_NOT_BENCHMARK, NULL);

    // profiling done only on first occlusion filled image generation
    Image *occlusionFilledLeft = OcclusionFillDriver(crossCheckLeft, benchmarkMode, profileInformation);
    Image *occlusionFilledRight = OcclusionFillDriver(crossCheckRight, DO_NOT_BENCHMARK, NULL);

    saveImage(OUTPUT_FILE_OCCULSION_FILLED_LEFT, occlusionFilledLeft);
    saveImage(OUTPUT_FILE_OCCULSION_FILLED_RIGHT, occlusionFilledRight);

    saveImage(OUTPUT_FILE_CROSS_CHECKING_LEFT, crossCheckLeft);
    saveImage(OUTPUT_FILE_CROSS_CHECKING_RIGHT, crossCheckRight);

    saveImage(OUTPUT_FILE_LEFT_DISPARITY, left_disparity_image);
    saveImage(OUTPUT_FILE_RIGHT_DISPARITY, right_disparity_image);

    printSummary(profileInformation);

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
