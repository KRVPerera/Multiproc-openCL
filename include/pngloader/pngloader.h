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
Image* createEmptyImage(unsigned width, unsigned height);

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
unsigned char *getNeighboursZeroPadding(Image *input, unsigned x, unsigned y);


float *getNeighboursZeroPaddingFloats(Image *input, unsigned x, unsigned y);

/**
 *
 * @param neighbours
 * @param filter
 * @param size
 * @return
 */
int applyFilterToNeighbours(unsigned char *neighbours, unsigned char *filter, int size);
float applyFilterToNeighboursFloat(float *neighbours, unsigned char *filter, int size);

/**
 * Apply a filter to an image
 * @param input
 * @param filter
 * @param filterDenominator
 * @param filterSize
 * @return
 */
Image *applyFilter(Image *input, unsigned char* filter, float filterDenominator, int filterSize);

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
