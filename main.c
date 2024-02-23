//
// Created by ruksh on 21/02/2024.
//

#include <stdio.h>
#include <stdlib.h>
#include <pngloader.h>

void handleImageLoad(Image *imgI);

void createSampleTestPng() {
    const char* filename = "test.png";

    /*generate some image*/
    unsigned width = 512, height = 512;
    unsigned char* image = malloc(width * height * 4);
    unsigned x, y;
    for(y = 0; y < height; y++)
        for(x = 0; x < width; x++) {
            image[4 * width * y + 4 * x + 0] = 255 * !(x & y);
            image[4 * width * y + 4 * x + 1] = x ^ y;
            image[4 * width * y + 4 * x + 2] = x | y;
            image[4 * width * y + 4 * x + 3] = 255;
        }

    /*run an example*/
    encodeOneStep(filename, image, width, height);

    free(image);
}

int main() {
    printf("Hello, World!\n");
    char* image0Name = "data/im0.png";
    char* image1Name = "data/im1.png";

    Image *im0 = loadImage(image0Name);
    handleImageLoad(im0);
    Image *im1 = loadImage(image1Name);
    handleImageLoad(im1);


    free(im0->image);
    free(im0);
    free(im1->image);
    free(im1);
    return 0;
}


