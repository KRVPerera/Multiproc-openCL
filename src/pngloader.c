//
// Created by ruksh on 21/02/2024.
//

#include <lodepng.h>
#include <pngloader.h>
#include <stdio.h>
#include <stdlib.h>

// TODO: do we need to filter alpha?
int applyFilterToNeighbours(const unsigned char *neighbours, const unsigned char *filter, const int size)
{
  int convolutionValue = 0;
  for (unsigned char index = 0; index < size * size; ++index) { convolutionValue += neighbours[index] * filter[index]; }
  return convolutionValue;
}

float applyFilterToNeighboursFloat(const float *neighbours, const unsigned char *filter, const int size)
{
  float convolutionValue = 0;
  for (unsigned char index = 0; index < size * size; ++index) {
    convolutionValue += neighbours[index] * (float)filter[index];
  }
  return convolutionValue;
}


float applyFilterForNonZeroFloat(const float *neighbours, const unsigned char *filter, const int size)
{
  float convolutionValue = 0;
  float prevNonZeroValue = 0;

  for (unsigned char index = 0; index < size * size; ++index) {
    if (neighbours[index] != 0) {
      prevNonZeroValue = neighbours[index];
      break;
    }
  }

  for (unsigned char index = 0; index < size * size; ++index) {
    float pixelValue = neighbours[index];
    if (pixelValue == 0) {
      pixelValue = prevNonZeroValue;
    } else {
      prevNonZeroValue = pixelValue;
    }
    convolutionValue += pixelValue * (float)filter[index];
  }
  return convolutionValue;
}

Image *readImage(const char *filename)
{
  unsigned error;
  unsigned char *image = 0;
  unsigned width, height;

  error = lodepng_decode32_file(&image, &width, &height, filename);
  if (error) printf("error %u: %s\n", error, lodepng_error_text(error));

  Image *img = malloc(sizeof(Image));
  img->image = image;
  img->width = width;
  img->height = height;
  img->error = error;

  handleImageLoad(img);
  return img;
}

Image *createEmptyImage(const unsigned width, const unsigned height)
{
  Image *img = malloc(sizeof(Image));
  img->image = malloc(width * height * 4);
  img->width = width;
  img->height = height;
  img->error = 0;
  return img;
}

/**
 * Generate grayscale image based on equation Y= 0.2126R + 0.7152G + 0.0722B
 * @param input
 * @param output
 */
Image *grayScaleImage(const Image *input)
{
  Image *output = createEmptyImage(input->width, input->height);
  for (unsigned y = 0; y < input->height; y++) {
    for (unsigned x = 0; x < input->width; x++) {
      size_t index = 4 * input->width * y + 4 * x;
      unsigned char r = input->image[index + 0];
      unsigned char g = input->image[index + 1];
      unsigned char b = input->image[index + 2];
      unsigned char a = input->image[index + 3];
      float gray = (float)(r * 0.2126 + g * 0.7152 + b * 0.0722);
      unsigned char grayChar = (unsigned char)gray;
      output->image[index + 0] = grayChar;
      output->image[index + 1] = grayChar;
      output->image[index + 2] = grayChar;
      output->image[index + 3] = a;
    }
  }
  return output;
}

/**
 * Y=0.2126R + 0.7152G + 0.0722B
 * @param input
 * @param output
 */
Image *grayScaleImage_MT(const Image *input)
{
  Image *output = createEmptyImage(input->width, input->height);

#pragma omp parallel for collapse(2)
  for (unsigned y = 0; y < input->height; y++) {
    for (unsigned x = 0; x < input->width; x++) {
      size_t index = 4 * input->width * y + 4 * x;
      unsigned char r = input->image[index + 0];
      unsigned char g = input->image[index + 1];
      unsigned char b = input->image[index + 2];
      unsigned char a = input->image[index + 3];
      float gray = (float)(r * 0.2126 + g * 0.7152 + b * 0.0722);
      unsigned char grayChar = (unsigned char)gray;
      output->image[index + 0] = grayChar;
      output->image[index + 1] = grayChar;
      output->image[index + 2] = grayChar;
      output->image[index + 3] = a;
    }
  }
  return output;
}

unsigned char *getNeighbourWindowWithMirroringUnsigned(const Image *input, const unsigned x, const unsigned y)
{
  unsigned char *neighbours = malloc(5 * 5 * sizeof(unsigned char));
  for (unsigned i = 0; i < 5; ++i) {
    int y1 = y - 2 + i;
    for (unsigned j = 0; j < 5; ++j) {
      int x1 = x - 2 + j;
      unsigned char neighboursIndex = 5 * i + j;
      if (x1 < 0) {
        x1 = abs(x1);
      } else if (x1 >= (int)input->width) {
        x1 = input->width + input->width - x1 - 2;
      }

      if (y1 < 0) {
        y1 = abs(y1);
      } else if (y1 >= (int)input->height) {
        y1 = input->height + input->height - y1 - 2;
      }

      unsigned char index = 4 * input->width * y1 + 4 * x1;
      neighbours[neighboursIndex] = input->image[index];
    }
  }
  return neighbours;
}

float *getNeighbourWindowWithMirroring(const Image *input, const unsigned x, const unsigned y, const int windowSize)
{
  float *neighbours = malloc(windowSize * windowSize * sizeof(float));
  for (int i = 0; i < windowSize; ++i) {
    int y1 = y - windowSize / 2 + i;
    for (int j = 0; j < windowSize; ++j) {
      int x1 = x - windowSize / 2 + j;
      unsigned char neighboursIndex = windowSize * i + j;
      if (x1 < 0) {
        x1 = abs(x1);
      } else if (x1 >= (int)input->width) {
        x1 = input->width + input->width - x1 - 2;
      }

      if (y1 < 0) {
        y1 = abs(y1);
      } else if (y1 >= (int)input->height) {
        y1 = input->height + input->height - y1 - 2;
      }

      size_t index = 4 * input->width * y1 + 4 * x1;
      neighbours[neighboursIndex] = input->image[index];
    }
  }
  return neighbours;
}

float *getNeighboursZeroPaddingFloats(const Image *input, const unsigned x, const unsigned y)
{
  float *neighbours = malloc(5 * 5 * sizeof(float));
  for (unsigned i = 0; i < 5; ++i) {
    int y1 = y - 2 + i;
    for (unsigned j = 0; j < 5; ++j) {
      int x1 = x - 2 + j;
      unsigned char neighboursIndex = 5 * i + j;
      if (x1 < 0) {
        x1 = abs(x1);
      } else if (x1 >= (int)input->width) {
        x1 = input->width + input->width - x1 - 2;
      }
      if (y1 < 0) {
        y1 = abs(y1);
      } else if (y1 >= (int)input->height) {
        y1 = input->height + input->height - y1 - 2;
      }
      size_t index = 4 * input->width * y1 + 4 * x1;
      neighbours[neighboursIndex] = input->image[index];
    }
  }
  return neighbours;
}

/**
 * Apply a filter to an BW image
 * @param input
 * @param filter
 * @param filterDenominator
 * @param filterSize
 * @return
 */
Image *applyFilter(const Image *input, const unsigned char *filter, const float filterDenominator, const int filterSize)
{
  Image *output = createEmptyImage(input->width, input->height);
  for (unsigned y = 0; y < input->height; y++) {
    size_t yIndex = 4 * input->width * y;
    for (unsigned x = 0; x < input->width; x++) {
      unsigned char *neighbours = getNeighbourWindowWithMirroringUnsigned(input, x, y);
      float *neighboursFloat = getNeighboursZeroPaddingFloats(input, x, y);
      float filterValueFloat = applyFilterToNeighboursFloat(neighboursFloat, filter, filterSize);
      unsigned char filterOut = MIN(255, (filterValueFloat / filterDenominator));
      size_t index = yIndex + 4 * x;
      output->image[index + 0] = filterOut;
      output->image[index + 1] = filterOut;
      output->image[index + 2] = filterOut;
      output->image[index + 3] = input->image[index + 3];
      free(neighbours);
    }
  }
  return output;
}

/**
 * Apply a filter to an BW image
 * @param input
 * @param filter
 * @param filterDenominator
 * @param filterSize
 * @return
 */
Image *
  applyFilter_MT(const Image *input, const unsigned char *filter, float filterDenominator, int filterSize)
{
  Image *output = createEmptyImage(input->width, input->height);

#pragma omp parallel for collapse(2)
  for (unsigned y = 0; y < input->height; y++) {
    for (unsigned x = 0; x < input->width; x++) {
      unsigned char *neighbours = getNeighbourWindowWithMirroringUnsigned(input, x, y);
      const float *neighboursFloat = getNeighboursZeroPaddingFloats(input, x, y);
      const float filterValueFloat = applyFilterToNeighboursFloat(neighboursFloat, filter, filterSize);
      const unsigned char filterOut = MIN(255, (filterValueFloat / filterDenominator));
      size_t index = 4 * input->width * y + 4 * x;
      output->image[index + 0] = filterOut;
      output->image[index + 1] = filterOut;
      output->image[index + 2] = filterOut;
      output->image[index + 3] = input->image[index + 3];
      free(neighbours);
    }
  }
  return output;
}


/**
 * Scale down to 1/16 taking every 4th row and column
 * @param input
 * @param output
 */
Image *resizeImage(const Image *input)
{
  Image *output = createEmptyImage(input->width / IMAGE_SCALE, input->height / IMAGE_SCALE);
  for (unsigned y = 0; y < input->height; y = y + IMAGE_SCALE) {
    for (unsigned x = 0; x < input->width; x = x + IMAGE_SCALE) {
      size_t index = 4 * input->width * y + 4 * x;
      size_t outIndex = 4 * output->width * (y / IMAGE_SCALE) + 4 * (x / IMAGE_SCALE);
      unsigned char r = input->image[index + 0];
      unsigned char g = input->image[index + 1];
      unsigned char b = input->image[index + 2];
      unsigned char a = input->image[index + 3];
      output->image[outIndex + 0] = r;
      output->image[outIndex + 1] = g;
      output->image[outIndex + 2] = b;
      output->image[outIndex + 3] = a;
    }
  }
  return output;
}

/**
 * Scale down to 1/16 taking every 4th row and column
 * @param input
 * @param output
 */
Image *resizeImage_MT(const Image *input)
{
  Image *output = createEmptyImage(input->width / IMAGE_SCALE, input->height / IMAGE_SCALE);

#pragma omp parallel for collapse(2)
  for (unsigned y = 0; y < input->height; y = y + IMAGE_SCALE) {
    for (unsigned x = 0; x < input->width; x = x + IMAGE_SCALE) {
      size_t index = 4 * input->width * y + 4 * x;
      size_t outIndex = 4 * output->width * (y / IMAGE_SCALE) + 4 * (x / IMAGE_SCALE);
      unsigned char r = input->image[index + 0];
      unsigned char g = input->image[index + 1];
      unsigned char b = input->image[index + 2];
      unsigned char a = input->image[index + 3];
      output->image[outIndex + 0] = r;
      output->image[outIndex + 1] = g;
      output->image[outIndex + 2] = b;
      output->image[outIndex + 3] = a;
    }
  }
  return output;
}

void saveImage(const char *filename, Image *img)
{
  /*Encode the image*/
  unsigned error = lodepng_encode32_file(filename, img->image, img->width, img->height);

  /*if there's an error, display it*/
  if (error) printf("error %u: %s\n", error, lodepng_error_text(error));
}

void handleImageLoad(Image *imgI)
{
  if (imgI->error) {
    printf("Error loading image\n");
    freeImage(imgI);
  }
}

void freeImage(Image *img)
{
  if (img == NULL) return;
  if (img->image != NULL) free(img->image);
  img->image = NULL;
  free(img);
  img = NULL;
}
