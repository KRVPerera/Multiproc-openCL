//
// Created by ruksh on 21/02/2024.
//

#include <stdio.h>
#include <stdlib.h>
#include <pngloader.h>
#include "config.h"

void createSampleTestPng() {
    const char* filename = "test.png";

    /*generate some image*/
    unsigned width = 512, height = 512;
    unsigned char* image = malloc(width * height * 4);
    unsigned x, y;
    for(y = 0; y < height; y++)
        for(x = 0; x < width; x++) {
            image[4 * width * y + 4 * x + 0] = 255 * !(x & y); // red
            image[4 * width * y + 4 * x + 1] = x ^ y;       // green
            image[4 * width * y + 4 * x + 2] = x | y;       // blue
            image[4 * width * y + 4 * x + 3] = 255;         // alpha
        }

    /*run an example*/
    encodeOneStep(filename, image, width, height);

    free(image);
}

int main() {
    printf("Starting Multiprocessor Programming project!\n");
    printf("Data folder %s\n", PROJECT_DATA_DIR);
    char* image0Name = "im0.png";
    char* image1Name = "im1.png";
    char imagePath0[256];  // Adjust the size based on your needs
    char imagePath1[256];
    snprintf(imagePath0, sizeof(imagePath0), "%s/sample/%s", PROJECT_DATA_DIR, image0Name);
    snprintf(imagePath1, sizeof(imagePath1), "%s/sample/%s", PROJECT_DATA_DIR, image1Name);

    printf("Image 0 path: %s\n", imagePath0);
    printf("Image 1 path: %s\n", imagePath1);

    Image *im0 = loadImage(imagePath0);
    Image *im1 = loadImage(imagePath1);

    Image* grayIm0 = createNewImage(im0->width, im0->height);
    Image* grayIm1 = createNewImage(im0->width, im0->height);
    Image* outputImageRed = createNewImageWithValue(im0->width, im0->height, 255, 0, 0, 255);

    getGrayScaleImage(im0, grayIm0);
    getGrayScaleImage(im1, grayIm1);
    saveImage("image_0_bw.png", outputImageRed);
    saveImage("grayIm0.png", grayIm0);
    saveImage("grayIm1.png", grayIm1);

    freeImage(im0);
    freeImage(im1);
    freeImage(outputImageRed);
    freeImage(grayIm0);
    freeImage(grayIm1);

    printf("Stopping Multiprocessor Programming project!\n");
    return 0;
}


