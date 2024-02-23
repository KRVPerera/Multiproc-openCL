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
