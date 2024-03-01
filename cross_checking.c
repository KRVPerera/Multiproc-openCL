#include <pngloader.h>
#include <stdio.h>

Image* CrossCheck(Image * image1, Image* image2, const char * outputImagePath, int threshold) {
    Image * crossCheckedImage = createEmptyImage(image2->width, image1->height);
    crossCheckedImage->width = image1->width;
    crossCheckedImage->height = image1->height;

    printf("Cross checking images %d\n", threshold);
    printf("Cross checking images %d\n", threshold);
    printf("Cross checking images %s\n", outputImagePath);
    return crossCheckedImage;

}