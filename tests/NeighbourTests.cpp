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
}

TEST_CASE("create a image and assign values", "[Image]") {
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