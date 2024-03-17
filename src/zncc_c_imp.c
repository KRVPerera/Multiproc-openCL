#include "pngloader.h"
#include <stdio.h>
#include <stdlib.h>
#include "config.h"
#include "util.h"
#include <math.h>

Image *Get_zncc_c_imp(const Image *image1, const Image *image2, const int direction) {
    Image *depth_image = createEmptyImage(image1->width, image1->height);

    const int height = image1->height;
    const int width = image1->width;

    for (int y = 0; y < height; ++y) {
        size_t tyIndex = 4 * y * width;
        for (int x = 0; x < width; ++x) {
            float bestDisp = 0;
            float max_zncc = 0;
            size_t index = tyIndex + 4 * x;

            float *image1Window = getNeighbourWindowWithMirroring(image1, x, y, ZNCC_WINDOW_SIZE);
            const float image1mean = Average(image1Window, ZNCC_WINDOW_SIZE * ZNCC_WINDOW_SIZE);

            for (int d = 0; d < MAX_DISP; ++d) {
                float *image2Window = getNeighbourWindowWithMirroring(image2, x - direction * d, y, ZNCC_WINDOW_SIZE);
                const float iamge2mean = Average(image2Window, ZNCC_WINDOW_SIZE * ZNCC_WINDOW_SIZE);

                float diffMultiSum = 0;
                float squaredSum2 = 0;
                float squaredSum1 = 0;
                for (int i = 0; i < ZNCC_WINDOW_SIZE * ZNCC_WINDOW_SIZE; ++i) {
                    const float firstDiff = image1Window[i] - image1mean;
                    const float firstDiffSquared = firstDiff * firstDiff;
                    const float secondDiff = image2Window[i] - iamge2mean;
                    const float secondDiffSquared = secondDiff * secondDiff;
                    const float diffMultiplied = firstDiff * secondDiff;
                    diffMultiSum += diffMultiplied;
                    squaredSum1 += firstDiffSquared;
                    squaredSum2 += secondDiffSquared;
                }
                float zncc = diffMultiSum / (sqrt(squaredSum1) * sqrt(squaredSum2));
                if (zncc > max_zncc) {
                    bestDisp = abs(d);
                    max_zncc = zncc;
                }
                free(image2Window);
            }
            free(image1Window);

            float normalizedDisp = (bestDisp / MAX_DISP) * 255;
            depth_image->image[index] = (unsigned char) normalizedDisp;
            depth_image->image[index + 1] = (unsigned char) normalizedDisp;
            depth_image->image[index + 2] = (unsigned char) normalizedDisp;
            depth_image->image[index + 3] = image1->image[index + 3];
        }
    }

    return depth_image;
}

Image *Get_zncc_c_imp_MT(const Image *image1, const Image *image2, const int direction) {
    Image *depth_image = createEmptyImage(image1->width, image1->height);

    const int height = image1->height;
    const int width = image1->width;

    #pragma omp parallel for simd shared(depth_image, image1, image2, direction) schedule(static, 4) collapse(2)
    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
            float bestDisp = 0;
            float max_zncc = 0;
            size_t index = 4 * y * width + 4 * x;

            float *image1Window = getNeighbourWindowWithMirroring(image1, x, y, ZNCC_WINDOW_SIZE);
            const float image1mean = Average(image1Window, ZNCC_WINDOW_SIZE * ZNCC_WINDOW_SIZE);

            for (int d = 0; d < MAX_DISP; ++d) {
                float *image2Window = getNeighbourWindowWithMirroring(image2, x - direction * d, y, ZNCC_WINDOW_SIZE);
                const float iamge2mean = Average(image2Window, ZNCC_WINDOW_SIZE * ZNCC_WINDOW_SIZE);
                float diffMultiSum = 0;
                float squaredSum2 = 0;
                float squaredSum1 = 0;
                for (int i = 0; i < ZNCC_WINDOW_SIZE * ZNCC_WINDOW_SIZE; ++i) {
                    const float firstDiff = image1Window[i] - image1mean;
                    const float firstDiffSquared = firstDiff * firstDiff;
                    const float secondDiff = image2Window[i] - iamge2mean;
                    const float secondDiffSquared = secondDiff * secondDiff;
                    const float diffMultiplied = firstDiff * secondDiff;
                    diffMultiSum += diffMultiplied;
                    squaredSum1 += firstDiffSquared;
                    squaredSum2 += secondDiffSquared;
                }
                float zncc = diffMultiSum / (sqrt(squaredSum1) * sqrt(squaredSum2));
                if (zncc > max_zncc) {
                    bestDisp = abs(d);
                    max_zncc = zncc;
                }
                free(image2Window);
            }
            free(image1Window);

            float normalizedDisp = (bestDisp / MAX_DISP) * 255;
            depth_image->image[index] = (unsigned char) normalizedDisp;
            depth_image->image[index + 1] = (unsigned char) normalizedDisp;
            depth_image->image[index + 2] = (unsigned char) normalizedDisp;
            depth_image->image[index + 3] = image1->image[index + 3];
        }
    }

    return depth_image;
}