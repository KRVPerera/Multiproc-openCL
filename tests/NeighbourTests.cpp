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
unsigned char *getNeighbourWindowWithMirroringUnsigned(Image *input, unsigned x, unsigned y);
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
    for (unsigned char i = 0; i < 5 * 5 * 4; i += 4) {
        im->image[i] = i;
        im->image[i + 1] = i;
        im->image[i + 2] = i;
        im->image[i + 3] = i;
    }
    for (unsigned char i = 0; i < 5 * 5 * 4; i += 4) {
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
    // i goes from 0 to 99 in steps of 4 eg: 0, 4, 8, 12, 16, 20, 24, 28, 32, 36, 40, 44, 48, 52, 56, 60, 64, 68, 72,
    //                                              76, 80, 84, 88, 92, 96
    // so the red channel will have 4(1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20,
    //                                               21, 22, 23, 24) and so on
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

    unsigned char *neigb00 = getNeighbourWindowWithMirroringUnsigned(im, 0, 0);
    int neighbourSum = 0;
    for (int i = 0; i < 5; ++i) {
        for (int j = 0; j < 5; ++j) {
            neighbourSum += neigb00[i * 5 + j];
        }
    }
    free(neigb00);
    REQUIRE(neighbourSum == 4 * (2 * (1 + 2 + 5 + 6 + 7 + 10 + 11 + 12) + 2 * (6 + 7 + 11 + 12)));

    unsigned char *neigb01 = getNeighbourWindowWithMirroringUnsigned(im, 1, 0);
    neighbourSum = 0;
    for (int i = 0; i < 5; ++i) {
        for (int j = 0; j < 5; ++j) {
            neighbourSum += neigb01[i * 5 + j];
        }
    }
    free(neigb01);
    REQUIRE(neighbourSum ==
            4 *
            (11 + 10 + 11 + 12 + 13 + 6 + 5 + 6 + 7 + 8 + 1 + 1 + 2 + 3 + 6 + 5 + 6 + 7 + 8 + 11 + 10 + 11 + 12 + 13));

    unsigned char *neigbMid = getNeighbourWindowWithMirroringUnsigned(im, 2, 2);
    neighbourSum = 0;
    for (int i = 0; i < 5; ++i) {
        for (int j = 0; j < 5; ++j) {
            neighbourSum += neigbMid[i * 5 + j];
        }
    }
    free(neigbMid);
    REQUIRE(neighbourSum == 4 *
                            (1 + 2 + 3 + 4 + 5 + 6 + 7 + 8 + 9 + 10 + 11 + 12 + 13 + 14 + 15 + 16 + 17 + 18 + 19 + 20 +
                             21 + 22 + 23 + 24));


    unsigned char *neigbLeftMiddle = getNeighbourWindowWithMirroringUnsigned(im, 0, 2);
    neighbourSum = 0;
    for (int i = 0; i < 5; ++i) {
        for (int j = 0; j < 5; ++j) {
            neighbourSum += neigbLeftMiddle[i * 5 + j];
        }
    }
    free(neigbLeftMiddle);
    REQUIRE(neighbourSum == 4 * (5 + 10 + 15 + 20 + 2 * (1 + 2 + 6 + 7 + 11 + 12 + 16 + 17 + 21 + 22)));

    unsigned char *neigbRightMiddle = getNeighbourWindowWithMirroringUnsigned(im, 4, 2);
    neighbourSum = 0;
    for (int i = 0; i < 5; ++i) {
        for (int j = 0; j < 5; ++j) {
            neighbourSum += neigbRightMiddle[i * 5 + j];
        }
    }
    free(neigbRightMiddle);
    REQUIRE(neighbourSum == 4 * (4 + 9 + 14 + 19 + 24 + 2 * (2 + 3 + 7 + 8 + 12 + 13 + 17 + 18 + 22 + 23)));


    unsigned char *neigbBottomLeft = getNeighbourWindowWithMirroringUnsigned(im, 0, 4);
    neighbourSum = 0;
    for (int i = 0; i < 5; ++i) {
        for (int j = 0; j < 5; ++j) {
            neighbourSum += neigbBottomLeft[i * 5 + j];
        }
    }
    free(neigbBottomLeft);
    REQUIRE(neighbourSum == 4 * (22 + 21 + 20 + 21 + 22 + 2 * (12 + 11 + 10 + 11 + 12 + 17 + 16 + 15 + 16 + 17)));


    unsigned char *neigbBottomMiddle = getNeighbourWindowWithMirroringUnsigned(im, 2, 4);
    neighbourSum = 0;
    for (int i = 0; i < 5; ++i) {
        for (int j = 0; j < 5; ++j) {
            neighbourSum += neigbBottomMiddle[i * 5 + j];
        }
    }
    free(neigbBottomMiddle);
    REQUIRE(neighbourSum == 4 * (20 + 21 + 22 + 23 + 24 + 2 * (10 + 11 + 12 + 13 + 14 + 15 + 16 + 17 + 18 + 19)));

    unsigned char *neigbBottomRight = getNeighbourWindowWithMirroringUnsigned(im, 4, 4);
    neighbourSum = 0;
    for (int i = 0; i < 5; ++i) {
        for (int j = 0; j < 5; ++j) {
            neighbourSum += neigbBottomRight[i * 5 + j];
        }
    }
    free(neigbBottomRight);
    REQUIRE(neighbourSum == 4 * (22 + 23 + 24 + 23 + 22 + 2 * (12 + 13 + 14 + 13 + 12 + 17 + 18 + 19 + 18 + 17)));

    freeImage(im);
}