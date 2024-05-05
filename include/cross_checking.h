#ifndef MULTIPROCOPENCL_CROSS_CHECKING_H
#define MULTIPROCOPENCL_CROSS_CHECKING_H

#include <pngloader.h>

/**
 * Cross check two images.
 * The caller is responsible for freeing the memory
 * @param image1
 * @param image2
 * @param threshold
 * @return Image with the cross checked values
 */
Image *CrossCheck(const Image *image1, const Image *image2, int threshold);

/**
 * Cross check two images using OpenMP
 * The caller is responsible for freeing the memory
 * @param image1
 * @param image2
 * @param threshold
 * @return image with the cross checked values
 */
Image *CrossCheck_MT(const Image *image1, const Image *image2, int threshold);

#endif// MULTIPROCOPENCL_CROSS_CHECKING_H
