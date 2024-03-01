#include <pngloader.h>
#include <stdio.h>
#include <stdlib.h>
#include "config.h"
#include <util.h>
#include <math.h>

Image *Get_zncc_c_imp(Image *image1, Image *image2) {
    Image *depth_image = createEmptyImage(image1->width, image1->height);

    int height = image1->height;
    int width = image1->width;

    for (int y = 0; y < height; ++y) {
        size_t tyIndex = 4 * y * width;
        for (int x = 0; x < width; ++x) {
            float bestDisp = 0;
            float max_zncc = 0;
            size_t index = tyIndex + 4 * x;

            float *image1Window = getZeroPaddedWindow(image1, x, y, ZNCC_WINDOW_SIZE);
            float image1mean = Average(image1Window, ZNCC_WINDOW_SIZE * ZNCC_WINDOW_SIZE);

            for (int d = 0; d < MAX_DISP; ++d) {
                float *image2Window = getZeroPaddedWindow(image2, x - d, y, ZNCC_WINDOW_SIZE);
                float iamge2mean = Average(image2Window, ZNCC_WINDOW_SIZE * ZNCC_WINDOW_SIZE);

                float diffMultiSum = 0;
                float squaredSum1 = 0;
                float squaredSum2 = 0;
                for (int i = 0; i < ZNCC_WINDOW_SIZE * ZNCC_WINDOW_SIZE; ++i) {
                    float firstDiff = image1Window[i] - image1mean;
                    float secondDiff = image2Window[i] - iamge2mean;
                    float firstDiffSquared = firstDiff * firstDiff;
                    float secondDiffSquared = secondDiff * secondDiff;
                    float diffMultiplied = firstDiff * secondDiff;
                    diffMultiSum += diffMultiplied;
                    squaredSum1 += firstDiffSquared;
                    squaredSum2 += secondDiffSquared;
                }
                float zncc = diffMultiSum / (sqrt(squaredSum1) * sqrt(squaredSum2));
                if (zncc > max_zncc) {
                    bestDisp = d;
                    max_zncc = zncc;
                }
                free(image2Window);
            }
            free(image1Window);

            depth_image->image[index] = (unsigned char) bestDisp;
            depth_image->image[index + 1] = (unsigned char) bestDisp;
            depth_image->image[index + 2] = (unsigned char) bestDisp;
            depth_image->image[index + 3] = image1->image[index + 3];
        }
    }

    return depth_image;
}