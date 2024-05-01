//
// Created by ruksh on 21/02/2024.
//

#ifndef MULTIPROCOPENCL_PNGLOADER_H
#define MULTIPROCOPENCL_PNGLOADER_H

#include <util.h>

#define IMAGE_SCALE 4
#define MIN(a, b) ((a) < (b) ? (a) : (b))

typedef struct Image
{
  unsigned char *image;
  unsigned width;
  unsigned height;
  unsigned error;
} Image;

/**
 * Create a new image in memory
 * The caller is responsible for freeing the memory
 * @param width width of the image
 * @param height height of the image
 * @return
 */
Image *createEmptyImage(unsigned width, unsigned height);

/**
 * Scale down an image by a factor of 16
 * The caller is responsible for freeing the memory
 * @param input input image
 * @return scaled down image
 */
Image *resizeImage(const Image *input);

/**
 * Scale down an image by a factor of 16 using OpenMP
 * The caller is responsible for freeing the memory
 * @param input input image
 * @return scaled down image
 */
Image *resizeImage_MT(const Image *input);


/**
 * Get the 25 (5x5) neighbours of a pixel in an image
 * Out of bounds neighbours will be replaced by mirrored values
 * Returned array is a unsigned char array. The caller is responsible for freeing the memory
 * @param input
 * @param x
 * @param y
 * @return
 */
unsigned char *getNeighbourWindowWithMirroringUnsigned(const Image *input, unsigned x, unsigned y);

/**
 * Get the 25 (5x5) neighbours of a pixel in an image. Out of bounds neighbours will be replaced by zero
 * Returned array is a float array. The caller is responsible for freeing the memory
 * @param input
 * @param x
 * @param y
 * @return
 */
float *getNeighboursZeroPaddingFloats(const Image *input, unsigned x, unsigned y);

/**
 * Get the 25 (5x5) neighbours of a pixel in an image. Out of bounds neighbours will be replaced by the mirrored value
 * Returned array is a float array. The caller is responsible for freeing the memory
 * @param input
 * @param x
 * @param y
 * @param windowSize
 * @return neighbours array of size windowSize*windowSize
 */
float *getNeighbourWindowWithMirroring(const Image *input, unsigned x, unsigned y, int windowSize);

/**
 * Apply a filter to the neighbour values of a pixel in an image
 * precondition: filterSize should be equal to the number of elements in the neighbours array
 * @param neighbours array of neighbour values
 * @param filter char array of size : size
 * @param size size of the filter
 * @return the result of the filter applied to the neighbours
 */
float applyFilterToNeighboursFloat(const float *neighbours, const unsigned char *filter, int size);


/**
 * Apply a filter to an BW image. Zero value neighbours will be replaced by first non zero value found in the whole
 * window
 *
 * @param neighbours
 * @param filter
 * @param size
 * @return
 */
float applyFilterForNonZeroFloat(const float *neighbours, const unsigned char *filter, int size);


/**
 * Apply a filter to an image (convolution) operator
 * @param input
 * @param filter
 * @param filterDenominator
 * @param filterSize
 * @return
 */
Image *applyFilter(const Image *input, const unsigned char *filter, float filterDenominator, int filterSize);

/**
 * Apply a filter to an image
 * @param input
 * @param filter
 * @param filterDenominator
 * @param filterSize
 * @return
 */
Image *applyFilter_MT(const Image *input, const unsigned char *filter, float filterDenominator, int filterSize);


/**
 * Convert an image to grayscale
 * @param input
 * @param output
 */
Image *grayScaleImage(const Image *input);

/**
 * Convert an image to grayscale
 * @param input
 * @param output
 */
Image *grayScaleImage_MT(const Image *input);

/**
 * Save raw pixels to disk as a PNG file with a single function call
 * @param filename
 * @param img
 */
void saveImage(const char *filename, Image *img);

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
Image *readImage(const char *filename);

#endif// MULTIPROCOPENCL_PNGLOADER_H
