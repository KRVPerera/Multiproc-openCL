#include "pngloader.h"
#include <stdlib.h>

Image* CrossCheck(const Image * image1, const Image* image2, const int threshold) {
    Image * crossCheckedImage = createEmptyImage(image1->width, image1->height);

    const int height = image1->height;
    const int width = image1->width;

    for (int i = 0; i < height; ++i) {
        for (int j = 0; j < width; ++j) {
            size_t index = 4 * i * width + 4 * j;
            crossCheckedImage->image[index + 3] = image1->image[index + 3]; // alpha channel
            if (abs(image1->image[index] - image2->image[index]) > threshold) {
                crossCheckedImage->image[index] = 0;
                crossCheckedImage->image[index + 1] = 0;
                crossCheckedImage->image[index + 2] = 0;
            } else {
                crossCheckedImage->image[index] = image1->image[index];
                crossCheckedImage->image[index + 1] = image1->image[index + 1];
                crossCheckedImage->image[index + 2] = image1->image[index + 2];
            }
        }
    }
    return crossCheckedImage;
}

Image* CrossCheck_MT(const Image * image1, const Image* image2, const int threshold) {
    Image * crossCheckedImage = createEmptyImage(image1->width, image1->height);

    const int height = image1->height;
    const int width = image1->width;

    #pragma omp parallel for collapse(2)
    for (int i = 0; i < height; ++i) {
        for (int j = 0; j < width; ++j) {
            size_t index = 4 * i * width + 4 * j;
            crossCheckedImage->image[index + 3] = image1->image[index + 3]; // alpha channel
            if (abs(image1->image[index] - image2->image[index]) > threshold) {
                crossCheckedImage->image[index] = 0;
                crossCheckedImage->image[index + 1] = 0;
                crossCheckedImage->image[index + 2] = 0;
            } else {
                crossCheckedImage->image[index] = image1->image[index];
                crossCheckedImage->image[index + 1] = image1->image[index + 1];
                crossCheckedImage->image[index + 2] = image1->image[index + 2];
            }
        }
    }
    return crossCheckedImage;
}
