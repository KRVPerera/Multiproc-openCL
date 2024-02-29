//
// Created by ruksh on 21/02/2024.
//

#include <lodepng.h>
#include <pngloader.h>
#include <stdio.h>
#include <stdlib.h>


// TODO: do we need to filter alpha?
int applyFilterToNeighbours(unsigned char *neighbours, unsigned char *filter, int size) {
    int convolutionValue = 0;
    for (unsigned char y = 0; y < size; ++y) {
        for (unsigned char x = 0; x < size; ++x) {
            int index = size * y + x;
            convolutionValue += neighbours[index] * filter[index];
        }
    }
    return convolutionValue;
}

Image *readImage(const char *filename) {
    unsigned error;
    unsigned char *image = 0;
    unsigned width, height;

    error = lodepng_decode32_file(&image, &width, &height, filename);
    if (error) printf("error %u: %s\n", error, lodepng_error_text(error));

    Image *img = malloc(sizeof(Image));
    img->image = image;
    img->width = width;
    img->height = height;
    img->error = error;

    handleImageLoad(img);
    return img;
}

Image *createEmptyImage(unsigned width, unsigned height) {
    Image *img = malloc(sizeof(Image));
    img->image = malloc(width * height * 4);
    img->width = width;
    img->height = height;
    img->error = 0;
    return img;
}

/**
 * Y=0.2126R + 0.7152G + 0.0722B
 * @param input
 * @param output
 */
Image *grayScaleImage(Image *input) {
    Image *output = createEmptyImage(input->width, input->height);
    for (unsigned y = 0; y < input->height; y++) {
        for (unsigned x = 0; x < input->width; x++) {
            size_t index = 4 * input->width * y + 4 * x;
            unsigned char r = input->image[index + 0];
            unsigned char g = input->image[index + 1];
            unsigned char b = input->image[index + 2];
            unsigned char a = input->image[index + 3];
            float gray = r * 0.2126 + g * 0.7152 + b * 0.0722;
            unsigned char grayChar = (unsigned char) gray;
            output->image[index + 0] = grayChar;
            output->image[index + 1] = grayChar;
            output->image[index + 2] = grayChar;
            output->image[index + 3] = a;
        }
    }
    return output;
}

/**
 * collect 25 neighbours of a pixel
 * @param input input image is a BW image
 * @param x
 * @param y
 * @return
 */
unsigned char *getNeighboursZeroPadding(Image *input, unsigned x, unsigned y) {
    unsigned char *neighbours = malloc(5 * 5);
    for (unsigned i = 0; i < 5; ++i) {
        for (unsigned j = 0; j < 5; ++j) {
            int y1 = y - 2 + i;
            int x1 = x - 2 + j;
            unsigned char neighboursIndex = 5 * i + j;
            if (x1 < 0 || x1 >= (int) input->width || y1 < 0 || y1 >= (int) input->height) {
                neighbours[neighboursIndex] = 100;
            } else {
                unsigned char index = 4 * input->width * y1 + 4 * x1;
                neighbours[neighboursIndex] = input->image[index];
            }
        }
    }
    return neighbours;
}

/**
 * Apply a filter to an BW image
 * @param input
 * @param filter
 * @param filterDenominator
 * @param filterSize
 * @return
 */
Image *applyFilter(Image *input, unsigned char *filter, int filterDenominator, int filterSize) {
    Image *output = createEmptyImage(input->width, input->height);
    for (unsigned y = 0; y < output->height; y++) {
        for (unsigned x = 0; x < output->width; x++) {
            unsigned char *neighbours = getNeighboursZeroPadding(input, x, y);
            int filterValue = applyFilterToNeighbours(neighbours, filter, filterSize);
            unsigned char filterOut = (unsigned char) (filterValue / filterDenominator);
            size_t index = 4 * output->width * y + 4 * x;
            output->image[index + 0] = filterOut;
            output->image[index + 1] = filterOut;
            output->image[index + 2] = filterOut;
            output->image[index + 3] = input->image[index + 3];
            free(neighbours);
        }
    }
    return output;
}


/**
 * Scale down to 1/16 taking every 4th row and column
 * @param input
 * @param output
 */
Image *resizeImage(Image *input) {
    Image *output = createEmptyImage(input->width / IMAGE_SCALE, input->height / IMAGE_SCALE);
    for (unsigned x = 0; x < input->width; x = x + IMAGE_SCALE) {
        for (unsigned y = 0; y < input->height; y = y + IMAGE_SCALE) {
            size_t index = 4 * input->width * y + 4 * x;
            size_t outIndex = 4 * output->width * (y / IMAGE_SCALE) + 4 * (x / IMAGE_SCALE);
            unsigned char r = input->image[index + 0];
            unsigned char g = input->image[index + 1];
            unsigned char b = input->image[index + 2];
            unsigned char a = input->image[index + 3];
            output->image[outIndex + 0] = r;
            output->image[outIndex + 1] = g;
            output->image[outIndex + 2] = b;
            output->image[outIndex + 3] = a;
        }
    }
    return output;
}

void saveImage(const char *filename, Image *img) {
    /*Encode the image*/
    unsigned error = lodepng_encode32_file(filename, img->image, img->width, img->height);

    /*if there's an error, display it*/
    if (error) printf("error %u: %s\n", error, lodepng_error_text(error));
}

void handleImageLoad(Image *imgI) {
    if (imgI->error) {
        printf("Error loading image\n");
        freeImage(imgI);
    }
}

void freeImage(Image *img) {
    if (img == NULL) return;
    if (img->image != NULL) free(img->image);
    img->image = NULL;
    free(img);
    img = NULL;
}

