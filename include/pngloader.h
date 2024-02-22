//
// Created by ruksh on 21/02/2024.
//

#ifndef MULTIPROCOPENCL_PNGLOADER_H
#define MULTIPROCOPENCL_PNGLOADER_H

void decodeOneStep(const char* filename);
void decodeTwoSteps(const char* filename);
void encodeOneStep(const char* filename, const unsigned char* image, unsigned width, unsigned height);
void encodeTwoSteps(const char* filename, const unsigned char* image, unsigned width, unsigned height);

#endif //MULTIPROCOPENCL_PNGLOADER_H
