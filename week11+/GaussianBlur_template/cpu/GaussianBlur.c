
// 512*512 image
#define BMP_FILE "lena.bmp"

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include <time.h>

#include "ImageProcessing.h"

void gaussian_blur(unsigned char *src_img, unsigned char *dst_img, int width, int height);

int main(int argc, char *argv[])
{
    struct timeval start, end, timer;
    BMPHEADER bmpHeader;
    unsigned char *image;
    unsigned char *blurred_img;

    image = read_bmp(BMP_FILE, &bmpHeader);

    blurred_img = (char *)malloc(bmpHeader.biSizeImage);

    gettimeofday(&start, NULL);
    // cpu gaussian blur function
    gaussian_blur(image, blurred_img, bmpHeader.biWidth, bmpHeader.biHeight);

    gettimeofday(&end, NULL);
    timersub(&end, &start, &timer);
    printf("CPUtime : %lf\n", (timer.tv_usec / 1000.0 + timer.tv_sec * 1000.0));

    write_bmp("blurred_lena.bmp", bmpHeader.biWidth, bmpHeader.biHeight, blurred_img);

    fflush(stdout);

    free(image);
    free(blurred_img);
    return 0;
}

void gaussian_blur(unsigned char *src_img, unsigned char *dst_img, int width, int height)
{
    float red = 0, green = 0, blue = 0;
    int row = 0, col = 0;
    int m, n, k;
    int pix;
    float mask[9][9] = {
        {0.011237, 0.011637, 0.011931, 0.012111, 0.012172, 0.012111, 0.011931, 0.011637, 0.011237},
        {0.011637, 0.012051, 0.012356, 0.012542, 0.012605, 0.012542, 0.012356, 0.012051, 0.011637},
        {0.011931, 0.012356, 0.012668, 0.012860, 0.012924, 0.012860, 0.012668, 0.012356, 0.011931},
        {0.012111, 0.012542, 0.012860, 0.013054, 0.013119, 0.013054, 0.012860, 0.012542, 0.012111},
        {0.012172, 0.012605, 0.012924, 0.013119, 0.013185, 0.013119, 0.012924, 0.012605, 0.012172},
        {0.012111, 0.012542, 0.012860, 0.013054, 0.013119, 0.013054, 0.012860, 0.012542, 0.012111},
        {0.011931, 0.012356, 0.012668, 0.012860, 0.012924, 0.012860, 0.012668, 0.012356, 0.011931},
        {0.011637, 0.012051, 0.012356, 0.012542, 0.012605, 0.012542, 0.012356, 0.012051, 0.011637},
        {0.011237, 0.011637, 0.011931, 0.012111, 0.012172, 0.012111, 0.011931, 0.011637, 0.011237}};

    // pixel for-loop (r:0~h, c:0~w)
    for (row = 0; row < height; row++)
    {
        for (col = 0; col < width; col++)
        {
            blue = green = red = 0;
            // gaussian matrix for-loop (m: 0~9, n: 0~9)
            for (m = 0; m < 9; m++)
            {
                for (n = 0; n < 9; n++)
                {
                    pix = (((row + m - 4) % height) * width) * 3 + ((col + n - 4) % width) * 3;
                    red += src_img[pix + 0] * mask[m][n];
                    green += src_img[pix + 1] * mask[m][n];
                    blue += src_img[pix + 2] * mask[m][n];
                }
            }
            // set result
            dst_img[(row * width + col) * 3 + 0] = red;
            dst_img[(row * width + col) * 3 + 1] = green;
            dst_img[(row * width + col) * 3 + 2] = blue;
        }
    }
}
