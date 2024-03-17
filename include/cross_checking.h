#ifndef MULTIPROCOPENCL_CROSS_CHECKING_H
#define MULTIPROCOPENCL_CROSS_CHECKING_H

Image* CrossCheck(const Image * image1, const Image* image2, const int threshold);
Image* CrossCheck_MT(const Image * image1, const Image* image2, const int threshold);

#endif //MULTIPROCOPENCL_CROSS_CHECKING_H
