#include "pngloader.h"
#include <stdlib.h>
#include <occlusion_filling.h>

unsigned char *getGaussianFilter();

Image *OcclusionFill(Image *image) {
    Image *occulsionFilledImage = createEmptyImage(image->width, image->height);
    unsigned char *gaussianFilter = getGaussianFilter();
    int height = image->height;
    int width = image->width;

    for (int i = 0; i < height; ++i) {
        for (int j = 0; j < width; ++j) {
            size_t index = 4 * i * width + 4 * j;
            occulsionFilledImage->image[index + 3] = 255;
            if (0 == image->image[index] && 0 == image->image[index + 1] && 0 == image->image[index + 2]) {

                float *neighboursFloat = getNeighboursZeroPaddingFloats(image, j, i);
                float filterValueFloat = applyFilterForNonZeroFloat(neighboursFloat, gaussianFilter, 5);
                unsigned char filterOut = MIN(255, (filterValueFloat / 273));

                occulsionFilledImage->image[index] = filterOut;
                occulsionFilledImage->image[index + 1] = filterOut;
                occulsionFilledImage->image[index + 2] = filterOut;

                free(neighboursFloat);
            } else {
                occulsionFilledImage->image[index] = image->image[index];
                occulsionFilledImage->image[index + 1] = image->image[index + 1];
                occulsionFilledImage->image[index + 2] = image->image[index + 2];
            }
        }
    }
    return occulsionFilledImage;
}

Image *OcclusionFill_MT(Image *image) {
    Image *occulsionFilledImage = createEmptyImage(image->width, image->height);
    const unsigned char *gaussianFilter = getGaussianFilter();
    const int height = image->height;
    const int width = image->width;


    #pragma omp parallel for shared(occulsionFilledImage, image)
    for (int i = 0; i < height; ++i) {
        for (int j = 0; j < width; ++j) {
            size_t index = 4 * i * width + 4 * j;
            occulsionFilledImage->image[index + 3] = 255;
            if (0 == image->image[index] && 0 == image->image[index + 1] && 0 == image->image[index + 2]) {

                float *neighboursFloat = getNeighboursZeroPaddingFloats(image, j, i);
                const float filterValueFloat = applyFilterForNonZeroFloat(neighboursFloat, gaussianFilter, 5);
                const unsigned char filterOut = MIN(255, (filterValueFloat / 273));

                occulsionFilledImage->image[index] = filterOut;
                occulsionFilledImage->image[index + 1] = filterOut;
                occulsionFilledImage->image[index + 2] = filterOut;

                free(neighboursFloat);
            } else {
                occulsionFilledImage->image[index] = image->image[index];
                occulsionFilledImage->image[index + 1] = image->image[index + 1];
                occulsionFilledImage->image[index + 2] = image->image[index + 2];
            }
        }
    }
    return occulsionFilledImage;
}