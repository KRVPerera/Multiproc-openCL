#include <catch2/catch_test_macros.hpp>

extern "C" {
struct Image {
    unsigned char *image;
    unsigned width;
    unsigned height;
    unsigned error;
};
Image *createEmptyImage(unsigned width, unsigned height);
void freeImage(Image *img);
unsigned char *getNeighboursZeroPadding(Image *input, unsigned x, unsigned y);
}

TEST_CASE("create a image and assign same value to red channel", "[Image]") {
    Image *im = createEmptyImage(5, 5);
    for (int i = 0; i < 25; ++i) {
        im->image[i] = 1;
    }
    for (int i = 0; i < 25; ++i) {
        REQUIRE(im->image[i] == 1);
    }
    for (int y = 0; y < im->height; ++y) {
        for (int x = 0; x < im->width; ++x) {
            int index = y * im->width + x;
            REQUIRE(im->image[index] == 1);
        }
    }
    freeImage(im);
}

TEST_CASE("create a image and assign different value to all channels", "[img_loader]") {
    Image *im = createEmptyImage(5, 5);
    for (unsigned char i = 0; i < 5*5*4; i += 4) {
        im->image[i] = i;
        im->image[i + 1] = i;
        im->image[i + 2] = i;
        im->image[i + 3] = i;
    }
    for (unsigned char i = 0; i < 5*5*4; i += 4) {
        REQUIRE(im->image[i] == i);
        REQUIRE(im->image[i + 1] == i);
        REQUIRE(im->image[i + 2] == i);
        REQUIRE(im->image[i + 3] == i);
    }
    for (int y = 0; y < im->height; ++y) {
        for (int x = 0; x < im->width; ++x) {
            int index = 4 * y * im->width + 4 * x;
            REQUIRE(im->image[index] == index);
            REQUIRE(im->image[index + 1] == index);
            REQUIRE(im->image[index + 2] == index);
            REQUIRE(im->image[index + 3] == index);
        }
    }
    freeImage(im);
}

TEST_CASE("test neighbours test", "[img_loader]") {
    Image *im = createEmptyImage(5, 5);
    for (unsigned char i = 0; i < 100; i += 4) {
        im->image[i] = i;
        im->image[i + 1] = i;
        im->image[i + 2] = i;
        im->image[i + 3] = i;
    }
    for (int y = 0; y < im->height; ++y) {
        for (int x = 0; x < im->width; ++x) {
            int index = 4 * y * im->width + 4 * x;
            REQUIRE(im->image[index] == index);
            REQUIRE(im->image[index + 1] == index);
            REQUIRE(im->image[index + 2] == index);
            REQUIRE(im->image[index + 3] == index);
        }
    }

    unsigned char *neigb00 = getNeighboursZeroPadding(im, 0, 0);
    int neighbourSum = 0;
    for (int i = 0; i < 5; ++i) {
        for (int j = 0; j < 5; ++j) {
            neighbourSum += neigb00[i * 5 + j];
        }
    }
    free(neigb00);
    REQUIRE(neighbourSum == 4*(1 + 2 + 5 + 6 + 7 + 10 + 11 + 12));

    unsigned char *neigb01 = getNeighboursZeroPadding(im, 1, 0);
    neighbourSum = 0;
    for (int i = 0; i < 5; ++i) {
        for (int j = 0; j < 5; ++j) {
            neighbourSum += neigb01[i * 5 + j];
        }
    }
    free(neigb01);
    REQUIRE(neighbourSum == 4*(1 + 2 + 5 + 6 + 7 + 10 + 11 + 12+3+8+13));

    unsigned char *neigbMid = getNeighboursZeroPadding(im, 2, 2);
    neighbourSum = 0;
    for (int i = 0; i < 5; ++i) {
        for (int j = 0; j < 5; ++j) {
            neighbourSum += neigbMid[i * 5 + j];
        }
    }
    free(neigbMid);
    REQUIRE(neighbourSum == 4*(1 + 2 + 3 + 4 + 5 + 6 + 7 + 8+9+10+11+12+13+14+15+16+17+18+19+20+21+22+23+24));


    unsigned char *neigbLeftMiddle = getNeighboursZeroPadding(im, 0, 2);
    neighbourSum = 0;
    for (int i = 0; i < 5; ++i) {
        for (int j = 0; j < 5; ++j) {
            neighbourSum += neigbLeftMiddle[i * 5 + j];
        }
    }
    free(neigbLeftMiddle);
    REQUIRE(neighbourSum == 4*(1 + 2 + 5 + 6 + 7 + 10 + 11 + 12 + 15 + 16 + 17 + 20 +21 +22));

    unsigned char *neigbRightMiddle = getNeighboursZeroPadding(im, 4, 2);
    neighbourSum = 0;
    for (int i = 0; i < 5; ++i) {
        for (int j = 0; j < 5; ++j) {
            neighbourSum += neigbRightMiddle[i * 5 + j];
        }
    }
    free(neigbRightMiddle);
    REQUIRE(neighbourSum == 4*(2 + 3 + 4 + 7 + 8 + 9 + 12 + 13 + 14 + 17 + 18 + 19 + 22 + 23 + 24));


    unsigned char *neigbBottomLeft = getNeighboursZeroPadding(im, 0, 4);
    neighbourSum = 0;
    for (int i = 0; i < 5; ++i) {
        for (int j = 0; j < 5; ++j) {
            neighbourSum += neigbBottomLeft[i * 5 + j];
        }
    }
    free(neigbBottomLeft);
    REQUIRE(neighbourSum == 4*(10 + 11 + 12 + 15 + 16 + 17 + 20+ 21 + 22));


    unsigned char *neigbBottomMiddle = getNeighboursZeroPadding(im, 2, 4);
    neighbourSum = 0;
    for (int i = 0; i < 5; ++i) {
        for (int j = 0; j < 5; ++j) {
            neighbourSum += neigbBottomMiddle[i * 5 + j];
        }
    }
    free(neigbBottomMiddle);
    REQUIRE(neighbourSum == 4*(10 + 11 + 12 + 13 + 14 + 15 + 16 + 17 + 18 + 19 + 20 + 21 + 22 + 23 + 24));

    unsigned char *neigbBottomRight = getNeighboursZeroPadding(im, 4, 4);
    neighbourSum = 0;
    for (int i = 0; i < 5; ++i) {
        for (int j = 0; j < 5; ++j) {
            neighbourSum += neigbBottomRight[i * 5 + j];
        }
    }
    free(neigbBottomRight);
    REQUIRE(neighbourSum == 4*(12 + 13 + 14 + 17 + 18 + 19 + 22 + 23 + 24));

    freeImage(im);
}