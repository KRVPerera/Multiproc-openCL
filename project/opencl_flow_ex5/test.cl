__constant sampler_t sampler = CLK_NORMALIZED_COORDS_FALSE | CLK_ADDRESS_CLAMP_TO_EDGE | CLK_FILTER_NEAREST;

__kernel void resize_image(__read_only image2d_t inputImage, __write_only image2d_t outputImage) {
    const int2 output_cord = (int2)(get_global_id(0), get_global_id(1));
    const int new_width = get_global_size(0);
    const int new_height = get_global_size(1);

    // Read the color pixel from the input image
    int2 input_image_cord = (int2)(output_cord.x * 4, output_cord.y * 4);
    float4 colorPixel = read_imagef(inputImage, sampler, input_image_cord);

    // Write the value to the output image
    write_imagef(outputImage, output_cord, colorPixel);
}

__kernel void color_to_gray(__read_only image2d_t inputImage, __write_only image2d_t outputImage) {
    const int2 pos = (int2)(get_global_id(0), get_global_id(1));

    // Read the color pixel from the input image
    float4 colorPixel = read_imagef(inputImage, sampler, pos);

    // Convert RGB to grayscale using luminance (Y = 0.2126*R + 0.7152*G + 0.0722*B)
    float grayscaleValue = 0.2126f * colorPixel.x + 0.7152f * colorPixel.y + 0.0722f * colorPixel.z;

    // Write the grayscale value to the output image
    write_imagef(outputImage, pos, (float4)(grayscaleValue, grayscaleValue, grayscaleValue, 1.0f));
}

__kernel void zncc(__read_only image2d_t inputImage1, __read_only image2d_t inputImage2, __write_only image2d_t outputImage) {
    const int2 pos = (int2)(get_global_id(0), get_global_id(1));

    const int width = get_global_size(0);
    const int height = get_global_size(1);
    const int WINDOW_SIZE = 15;
    const int WINDOW_HALF_SIZE = 7;
    float4 sum = 0.0;
    for (int i = -WINDOW_HALF_SIZE; i <= WINDOW_HALF_SIZE; ++i) {
        for (int j = -WINDOW_HALF_SIZE; j <= WINDOW_HALF_SIZE; ++j) {
            float bestDisp = 0;
            float max_zncc = 0;
            const int2 offsetPos = pos + (int2)(i, j);

            const float4 color = read_imagef(inputImage1, sampler, offsetPos);
            sum += color;
        }
    }

    const float4 image1Mean = sum / (WINDOW_SIZE * WINDOW_SIZE);
    float4 sum2 = 0.0;
    const int MAX_DISP = 65;
    for (int d = 0; d < MAX_DISP; ++d) {
        for (int i = -WINDOW_HALF_SIZE; i <= WINDOW_HALF_SIZE; ++i) {
            for (int j = -WINDOW_HALF_SIZE; j <= WINDOW_HALF_SIZE; ++j) {
                const int2 offsetPos = pos + (int2)(i, j);

                const float4 color = read_imagef(inputImage2, sampler, offsetPos);
                sum2 += color;
            }
        }



    }

    const float4 result = sum;
    write_imagef(outputImage, pos, result);
}