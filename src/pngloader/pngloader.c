//
// Created by ruksh on 21/02/2024.
//

#include <lodepng.h>
#include <pngloader.h>
#include <stdio.h>
#include <stdlib.h>


int *applyFilterToNeighbours(unsigned char *neighbours, unsigned char *filter, int size);

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
    for (unsigned x = 0; x < input->width; x++) {
        for (unsigned y = 0; y < input->height; y++) {
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

unsigned char * getNeighboursZeroPadding(Image *input, unsigned x, unsigned y) {
    unsigned char * neighbours = malloc(5 * 5 * 4);
    for (unsigned i = 0; i < 5; ++i) {
        for (unsigned j = 0; j < 5; ++j) {
            int x1 = x - 2 + i;
            int y1 = y - 2 + j;
            unsigned char neighboursIndex = 4 * 5 * i + 4 * j;
            if (x1 < 0 || x1 >= (int)input->width || y1 < 0 || y1 >= (int)input->height) {
                neighbours[neighboursIndex + 0] = 0;
                neighbours[neighboursIndex + 1] = 0;
                neighbours[neighboursIndex + 2] = 0;
                neighbours[neighboursIndex + 3] = 0;
            } else {
                unsigned char index = 4 * input->width * y1 + 4 * x1;
                neighbours[neighboursIndex + 0] = input->image[index + 0];
                neighbours[neighboursIndex + 1] = input->image[index + 1];
                neighbours[neighboursIndex + 2] = input->image[index + 2];
                neighbours[neighboursIndex + 3] = input->image[index + 3];
            }
        }
    }
    return neighbours;
}

Image *applyFilter(Image *input, unsigned char* filter, int filterDenominator, int filterSize) {
    Image *output = createEmptyImage(input->width, input->height);
    for (unsigned x = 0; x < output->width; x++) {
        for (unsigned y = 0; y < output->height; y++) {
            unsigned char * neighbours = getNeighboursZeroPadding(input, x, y);
            size_t index = 4 * output->width * y + 4 * x;
            int * filteredNeighbours = applyFilterToNeighbours(neighbours, filter, filterSize);
            output->image[index + 0] = filteredNeighbours[0] / filterDenominator;
            output->image[index + 1] = filteredNeighbours[1] / filterDenominator;
            output->image[index + 2] = filteredNeighbours[2] / filterDenominator;
            output->image[index + 3] = input->image[index + 3];
            free(neighbours);
        }
    }
    return output;
}


int *applyFilterToNeighbours(unsigned char *neighbours, unsigned char *filter, int size) {
    int *filteredNeighbours = malloc(sizeof(int) * 3);
        int r_sum = 0;
        int g_sum = 0;
        int b_sum = 0;
        for (int j = 0; j < size; ++j) {
            for (int k = 0; k < size; ++k) {
                r_sum += neighbours[4 * size * j + 4 * k + 0] * filter[size * j + k];
                g_sum += neighbours[4 * size * j + 4 * k + 1] * filter[size * j + k];
                b_sum += neighbours[4 * size * j + 4 * k + 2] * filter[size * j + k];
            }
        }
        filteredNeighbours[0] = r_sum;;
        filteredNeighbours[1] = g_sum;;
        filteredNeighbours[2] = b_sum;;

    return filteredNeighbours;
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

