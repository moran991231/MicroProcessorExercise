#define BMP_FILE "lena.bmp"

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include <time.h>

#include "ImageProcessing.h"

void gaussian_blur(unsigned char*, unsigned char*, int, int);

int main(int argc, char* argv[]) {
    struct timeval start, end, timer;
    BMPHEADER bmpHeader;
    unsigned char *image, *blured_img;

    image = read_bmp(BMP_FILE, &bmpHeader);

    blured_img = (char*)malloc(bmpHeader.biSizeImage);

    gettimeofday(&start, NULL);
    gaussian_blur(image, blured_img, bmpHeader.biWidth, bmpHeader.biHeight);

    gettimeofday(&end, NULL);
    timersub(&end, &start, &timer);
    printf("CPUtime: %lf\n", (timer.tv_usec / 1000.0 + timer.tv_sec * 1000.0));

    write_bmp("blured_lena.bmp", bmpHeader.biWidth, bmpHeader.biHeight, blured_img);

    fflush(stdout);

    free(image);
    free(blured_img);
    return 0;
}

void gaussian_blur(unsigned char* src_img, unsigned char* dst_img, int width, int height) {
    int row = 0, col = 0;
    float red = 0, green = 0, blue = 0;
    int m, n, k;
    int pix;
    float mask[9][9] = {{0.011237f, 0.011637f, 0.011931f, 0.012111f, 0.012172f, 0.012111f,
                         0.011931f, 0.011637f, 0.011237f},
                        {0.011637f, 0.012051f, 0.012356f, 0.012542f, 0.012605f, 0.012542f,
                         0.012356f, 0.012051f, 0.011637f},
                        {0.011931f, 0.012356f, 0.012668f, 0.012860f, 0.012924f, 0.012860f,
                         0.012668f, 0.012356f, 0.011931f},
                        {0.012111f, 0.012542f, 0.012860f, 0.013054f, 0.013119f, 0.013054f,
                         0.012860f, 0.012542f, 0.012111f},
                        {0.012172f, 0.012605f, 0.012924f, 0.013119f, 0.013185f, 0.013119f,
                         0.012924f, 0.012605f, 0.012172f},
                        {0.012111f, 0.012542f, 0.012860f, 0.013054f, 0.013119f, 0.013054f,
                         0.012860f, 0.012542f, 0.012111f},
                        {0.011931f, 0.012356f, 0.012668f, 0.012860f, 0.012924f, 0.012860f,
                         0.012668f, 0.012356f, 0.011931f},
                        {0.011637f, 0.012051f, 0.012356f, 0.012542f, 0.012605f, 0.012542f,
                         0.012356f, 0.012051f, 0.011637f},
                        {0.011237f, 0.011637f, 0.011931f, 0.012111f, 0.012172f, 0.012111f,
                         0.011931f, 0.011637f, 0.011237f}};
    for (row = 0; row < height; row++) {
        for (col = 0; col < width; col++) {
            blue = green = red = 0;

            // convolution
            for (m = 0; m < 9; m++) {
                for (n = 0; n < 9; n++) {
                    pix = ((row + m - 4) % height) * width * 3 + ((col + n - 4) % width) * 3;
                    red += src_img[pix + 0] * mask[m][n];
                    green += src_img[pix + 0] * mask[m][n];
                    blue += src_img[pix + 2] * mask[m][n];
                }
            }

            dst_img[(row * width + col) * 3 + 0] = red;
            dst_img[(row * width + col) * 3 + 1] = green;
            dst_img[(row * width + col) * 3 + 2] = blue;
        }
    }
}