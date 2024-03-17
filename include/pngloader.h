//
// Created by ruksh on 21/02/2024.
//

#ifndef MULTIPROCOPENCL_PNGLOADER_H
#define MULTIPROCOPENCL_PNGLOADER_H

#define IMAGE_SCALE 4
#define MIN(a, b) ((a) < (b) ? (a) : (b))

typedef struct Image {
    unsigned char* image;
    unsigned width;
    unsigned height;
    unsigned error;
} Image;

/**
 * Create a new image in memory
 * @param width
 * @param height
 * @return
 */
Image* createEmptyImage(unsigned width, const unsigned height);

/**
 * Scale down an image by a factor of 16
 * @param input
 * @return
 */
Image *resizeImage(Image *input);


/**
 *
 * @param input
 * @param x
 * @param y
 * @return
 */
unsigned char *getNeighbourWindowWithMirroringUnsigned(Image *input, const unsigned x, const unsigned y);


float *getNeighboursZeroPaddingFloats(Image *input, const unsigned x, const unsigned y);

float *getNeighbourWindowWithMirroring(Image *input, const unsigned x, const unsigned y, const int windowSize);

/**
 *
 * @param neighbours
 * @param filter
 * @param size
 * @return
 */
float applyFilterToNeighboursFloat(float *neighbours, const unsigned char *filter, const int size);


/**
 * Apply a filter to an BW image
 * @param neighbours
 * @param filter
 * @param size
 * @return
 */
float applyFilterForNonZeroFloat(float *neighbours, const unsigned char *filter, const int size);


/**
 * Apply a filter to an image
 * @param input
 * @param filter
 * @param filterDenominator
 * @param filterSize
 * @return
 */
Image *applyFilter(Image *input, const unsigned char* filter, const float filterDenominator, const int filterSize);

/**
 * Convert an image to grayscale
 * @param input
 * @param output
 */
Image *grayScaleImage(Image *input);

/**
 * Save raw pixels to disk as a PNG file with a single function call
 * @param filename
 * @param img
 */
void saveImage(const char *filename, Image* img);

/**
 * Free memory of image
 * @param img
 */
void freeImage(Image *img);

/**
 * Handle image loading errors free memory if error
 * @param imgI
 */
void handleImageLoad(Image *imgI);

/**
 * Load PNG file from disk to memory
 * @param filename
 * @return
 */
Image* readImage(const char *filename);

#endif //MULTIPROCOPENCL_PNGLOADER_H
