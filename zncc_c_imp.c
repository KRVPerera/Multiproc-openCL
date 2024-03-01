#include <pngloader.h>
#include <stdio.h>

Image* Get_zncc_c_imp(Image* image1, Image* image2, const char * outputImagePath) {
    Image * depth_image = createEmptyImage(image2->width, image1->height);
    depth_image->width = image1->width;
    depth_image->height = image1->height;

    

    printf("Depth image path: %s\n", outputImagePath);
    return depth_image;

}