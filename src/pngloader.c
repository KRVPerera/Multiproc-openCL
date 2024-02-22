//
// Created by ruksh on 21/02/2024.
//

#include <pngloader.h>
#include <lodepng.h>
#include <stdio.h>
#include <stdlib.h>

/*
Example 1
Decode from disk to raw pixels with a single function call
*/
void decodeOneStep(const char* filename) {
    unsigned error;
    unsigned char* image = 0;
    unsigned width, height;

    error = lodepng_decode32_file(&image, &width, &height, filename);
    if(error) printf("error %u: %s\n", error, lodepng_error_text(error));

    /*use image here*/

    free(image);
}

/*
Example 2
Load PNG file from disk to memory first, then decode to raw pixels in memory.
*/
void decodeTwoSteps(const char* filename) {
    unsigned error;
    unsigned char* image = 0;
    unsigned width, height;
    unsigned char* png = 0;
    size_t pngsize;

    error = lodepng_load_file(&png, &pngsize, filename);
    if(!error) error = lodepng_decode32(&image, &width, &height, png, pngsize);
    if(error) printf("error %u: %s\n", error, lodepng_error_text(error));

    free(png);

    /*use image here*/

    free(image);
}

/*
Example 1
Encode from raw pixels to disk with a single function call
The image argument has width * height RGBA pixels or width * height * 4 bytes
*/
void encodeOneStep(const char* filename, const unsigned char* image, unsigned width, unsigned height) {
    /*Encode the image*/
    unsigned error = lodepng_encode32_file(filename, image, width, height);

    /*if there's an error, display it*/
    if(error) printf("error %u: %s\n", error, lodepng_error_text(error));
}

/*
Example 2
Encode from raw pixels to an in-memory PNG file first, then write it to disk
The image argument has width * height RGBA pixels or width * height * 4 bytes
*/
void encodeTwoSteps(const char* filename, const unsigned char* image, unsigned width, unsigned height) {
    unsigned char* png;
    size_t pngsize;

    unsigned error = lodepng_encode32(&png, &pngsize, image, width, height);
    if(!error) lodepng_save_file(png, pngsize, filename);

    /*if there's an error, display it*/
    if(error) printf("error %u: %s\n", error, lodepng_error_text(error));

    free(png);
}