#pragma OPENCL EXTENSION cl_khr_fp64 : enable

__kernel void kernel_blur(__global unsigned char *src, __global unsigned char *dst,
                          const int width, const int height)
{
    int row = get_global_id(0) / width;
    int col = get_global_id(0) % width;
    int pix;

    float red = 0, green = 0, blue = 0;
    int m, n, k;

    float mask[9][9] = {
        {0.011237f, 0.011637f, 0.011931f, 0.012111f, 0.012172f, 0.012111f, 0.011931f, 0.011637f, 0.011237f},
        {0.011637f, 0.012051f, 0.012356f, 0.012542f, 0.012605f, 0.012542f, 0.012356f, 0.012051f, 0.011637f},
        {0.011931f, 0.012356f, 0.012668f, 0.012860f, 0.012924f, 0.012860f, 0.012668f, 0.012356f, 0.011931f},
        {0.012111f, 0.012542f, 0.012860f, 0.013054f, 0.013119f, 0.013054f, 0.012860f, 0.012542f, 0.012111f},
        {0.012172f, 0.012605f, 0.012924f, 0.013119f, 0.013185f, 0.013119f, 0.012924f, 0.012605f, 0.012172f},
        {0.012111f, 0.012542f, 0.012860f, 0.013054f, 0.013119f, 0.013054f, 0.012860f, 0.012542f, 0.012111f},
        {0.011931f, 0.012356f, 0.012668f, 0.012860f, 0.012924f, 0.012860f, 0.012668f, 0.012356f, 0.011931f},
        {0.011637f, 0.012051f, 0.012356f, 0.012542f, 0.012605f, 0.012542f, 0.012356f, 0.012051f, 0.011637f},
        {0.011237f, 0.011637f, 0.011931f, 0.012111f, 0.012172f, 0.012111f, 0.011931f, 0.011637f, 0.011237f}};
    // convolution
    for (m = 0; m < 9; m++)
    {
        for (n = 0; n < 9; n++)
        {
            pix = ((row + m - 4) % height) * width * 3 + ((col + n - 4) % width) * 3;
            red += src[pix + 0] * mask[m][n];
            green += src[pix + 0] * mask[m][n];
            blue += src[pix + 2] * mask[m][n];
        }
    }

    dst[(row * width + col) * 3 + 0] = red;
    dst[(row * width + col) * 3 + 1] = green;
    dst[(row * width + col) * 3 + 2] = blue;
}