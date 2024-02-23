//
// Created by ruksh on 21/02/2024.
//

#include <lodepng.h>
#include <pngloader.h>
#include <stdio.h>
#include <stdlib.h>


Image* loadImage(const char *filename)
{
    unsigned error;
    unsigned char *image = 0;
    unsigned width, height;

    error = lodepng_decode32_file(&image, &width, &height, filename);
    if (error) printf("error %u: %s\n", error, lodepng_error_text(error));

    Image* img = malloc(sizeof(Image));
    img->image = image;
    img->width = width;
    img->height = height;
    img->error = error;

    handleImageLoad(img);
    return img;
}

Image* createNewImage(unsigned width, unsigned height) {
    Image* img = malloc(sizeof(Image));
    img->image = malloc(width * height * 4);
    img->width = width;
    img->height = height;
    img->error = 0;
    return img;
}

void getGrayScaleImage(Image* input, Image* output) {
    for(unsigned x = 0; x < input->width; x++)
        for(unsigned y = 0; y < input->height; y++) {
            unsigned char r = input->image[4 * input->width * y + 4 * x + 0];
            unsigned char g = input->image[4 * input->width * y + 4 * x + 1];
            unsigned char b = input->image[4 * input->width * y + 4 * x + 2];
            unsigned char a = input->image[4 * input->width * y + 4 * x + 3];
            unsigned char gray = (r + g + b) / 3;
            output->image[4 * input->width * y + 4 * x + 0] = gray;
            output->image[4 * input->width * y + 4 * x + 1] = gray;
            output->image[4 * input->width * y + 4 * x + 2] = gray;
            output->image[4 * input->width * y + 4 * x + 3] = a;
        }
}

Image* createNewImageWithValue(unsigned width, unsigned height, int value) {
    Image* img = malloc(sizeof(Image));
    img->image = malloc(width * height * 4);
    img->width = width;
    img->height = height;
    for(unsigned x = 0; x < width; x++)
        for(unsigned y = 0; y < height; y++) {
            img->image[4 * width * y + 4 * x + 0] = value;
            img->image[4 * width * y + 4 * x + 1] = value;
            img->image[4 * width * y + 4 * x + 2] = value;
            img->image[4 * width * y + 4 * x + 3] = value;
        }
    img->error = 0;
    return img;
}

void saveImage(const char *filename, Image* img)
{
    /*Encode the image*/
    unsigned error = lodepng_encode32_file(filename, img->image, img->width, img->height);

    /*if there's an error, display it*/
    if (error) printf("error %u: %s\n", error, lodepng_error_text(error));
}

void handleImageLoad(Image *imgI) {
    if (imgI->error) {
        printf("Error loading image\n");
        freeImage(imgI);
    } else {
        printf("Image loaded\n");
    }
}

void freeImage(Image *img) {
    if (img == NULL) return;
    if (img->image != NULL) free(img->image);
    img->image = NULL;
    free(img);
    img = NULL;
}

/*
Example 1
Decode from disk to raw pixels with a single function call
*/
void decodeOneStep(const char *filename)
{
  unsigned error;
  unsigned char *image = 0;
  unsigned width, height;

  error = lodepng_decode32_file(&image, &width, &height, filename);
  if (error) printf("error %u: %s\n", error, lodepng_error_text(error));

  /*use image here*/

  free(image);
}

/*
Example 2
Load PNG file from disk to memory first, then decode to raw pixels in memory.
*/
void decodeTwoSteps(const char *filename)
{
  unsigned error;
  unsigned char *image = 0;
  unsigned width, height;
  unsigned char *png = 0;
  size_t pngsize;

  error = lodepng_load_file(&png, &pngsize, filename);
  if (!error) error = lodepng_decode32(&image, &width, &height, png, pngsize);
  if (error) printf("error %u: %s\n", error, lodepng_error_text(error));

  free(png);

  /*use image here*/

  free(image);
}

/*
Example 1
Encode from raw pixels to disk with a single function call
The image argument has width * height RGBA pixels or width * height * 4 bytes
*/
void encodeOneStep(const char *filename, const unsigned char *image, unsigned width, unsigned height)
{
  /*Encode the image*/
  unsigned error = lodepng_encode32_file(filename, image, width, height);

  /*if there's an error, display it*/
  if (error) printf("error %u: %s\n", error, lodepng_error_text(error));
}

/*
Example 2
Encode from raw pixels to an in-memory PNG file first, then write it to disk
The image argument has width * height RGBA pixels or width * height * 4 bytes
*/
void encodeTwoSteps(const char *filename, const unsigned char *image, unsigned width, unsigned height)
{
  unsigned char *png;
  size_t pngsize;

  unsigned error = lodepng_encode32(&png, &pngsize, image, width, height);
  if (!error) lodepng_save_file(png, pngsize, filename);

  /*if there's an error, display it*/
  if (error) printf("error %u: %s\n", error, lodepng_error_text(error));

  free(png);
}