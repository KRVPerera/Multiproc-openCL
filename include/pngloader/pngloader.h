//
// Created by ruksh on 21/02/2024.
//

#ifndef MULTIPROCOPENCL_PNGLOADER_H
#define MULTIPROCOPENCL_PNGLOADER_H

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
Image* createNewImage(unsigned width, unsigned height);

/**
 * Create a new image in memory with a single value
 * @param width
 * @param height
 * @param value
 * @return
 */
Image* createNewImageWithValue(unsigned width, unsigned height, int value);

/**
 * Convert an image to grayscale
 * @param input
 * @param output
 */
void getGrayScaleImage(Image* input, Image* output);

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
Image* loadImage(const char *filename);

/**
 * Decode from disk to raw pixels with a single function call
 * @param filename
 */
void decodeOneStep(const char* filename);

/**
 * Load PNG file from disk to memory first, then decode to raw pixels in memory.
 * @param filename
 */
void decodeTwoSteps(const char* filename);

/**
 * Encode from raw pixels to disk with a single function call
 * The image argument has width * height RGBA pixels or width * height * 4 bytes
 * @param filename
 * @param image
 * @param width
 * @param height
 */
void encodeOneStep(const char* filename, const unsigned char* image, unsigned width, unsigned height);

/**
 * Encode from raw pixels to disk with a two function calls
 * The image argument has width * height RGBA pixels or width * height * 4 bytes
 * @param filename
 * @param image
 * @param width
 * @param height
 */
void encodeTwoSteps(const char* filename, const unsigned char* image, unsigned width, unsigned height);

#endif //MULTIPROCOPENCL_PNGLOADER_H
