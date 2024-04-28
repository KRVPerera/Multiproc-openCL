#ifndef MULTIPROCOPENCL_OCCULSION_FILLING_H
#define MULTIPROCOPENCL_OCCULSION_FILLING_H

#include <pngloader.h>

/**
 * Fill in occlusions in an image
 * The caller is responsible for freeing the memory
 * @param image
 * @return
 */
Image *OcclusionFill(Image *image);

/**
 * Fill in occlusions in an image using OpenMP
 * The caller is responsible for freeing the memory
 * @param image
 * @return
 */
Image *OcclusionFill_MT(Image *image);

#endif// MULTIPROCOPENCL_OCCULSION_FILLING_H
