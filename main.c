//
// Created by ruksh on 21/02/2024.
//

#include <stdio.h>
#include <stdlib.h>
#include <pngloader.h>

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
    char* image0Name = "im0.png";
    char* image1Name = "im1.png";

    Image *im0 = loadImage(image0Name);
    Image *im1 = loadImage(image1Name);

    Image* grayIm0 = createNewImage(im0->width, im0->height);
    Image* grayIm1 = createNewImage(im0->width, im0->height);
    Image* outputImageRed = createNewImageWithValue(im0->width, im0->height, 255, 0, 0, 255);

    getGrayScaleImage(im0, grayIm0);
    getGrayScaleImage(im1, grayIm1);
    saveImage("outputImageRed.png", outputImageRed);
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


