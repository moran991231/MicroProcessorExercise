#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include <time.h>

#include "ImageProcessing.h"

void applyGrayScale_cpu(unsigned char *src, unsigned char *dst, int w, int h);
void rotate_cpu(unsigned char *src, unsigned char *dst, int w, int h);

int main(int argc, char *argv[]) {
    struct timeval start, end, timer_cpu;
    BMPHEADER bmpHeader;
    unsigned char *image, *ret_image;
    if (argc != 4) {
        printf("./openCLFilter [g/r] [input file name] [output file name]");
        return 0;
    }

    char *input_file_name = argv[2], *output_file_name = argv[3];
    image = read_bmp(input_file_name, &bmpHeader);
    ret_image = (char *)malloc(bmpHeader.biSizeImage);
    int w = bmpHeader.biWidth, h = bmpHeader.biHeight;
    memset(ret_image, 0, bmpHeader.biSizeImage);

    // cpu processing begin
    gettimeofday(&start, NULL);  // timer start
    switch (argv[1][0]) {
        case 'g':
        case 'G':
            applyGrayScale_cpu(image, ret_image, w, h);
            break;
        case 'r':
        case 'R':
            rotate_cpu(image, ret_image, w, h);
            w = bmpHeader.biHeight;
            h = bmpHeader.biWidth;
            break;
    }
    gettimeofday(&end, NULL);  // timer end
    timersub(&end, &start, &timer_cpu);
    printf("CPUtime : %lf\n", (timer_cpu.tv_usec / 1000.0 + timer_cpu.tv_sec * 1000.0));
    // cpu processing end

    write_bmp(output_file_name, w, h, ret_image);

    fflush(stdout);

    free(image);
    free(ret_image);
    return 0;
}

void applyGrayScale_cpu(unsigned char *src, unsigned char *dst, int w, int h) {
    float red, green, blue, gray;
    int pix, i;
    for (i = 0; i < w * h; i++) {
        pix = i * 3;
        red = src[pix + 0] * 0.2126f;
        green = src[pix + 1] * 0.7152f;
        blue = src[pix + 2] * 0.0722f;
        gray = red + green + blue;
        dst[pix + 0] = dst[pix + 1] = dst[pix + 2] = gray;
    }
}

void rotate_cpu(unsigned char *src, unsigned char *dst, int w, int h) {
    // rotation matrix
    /*
    |cos(a) -sin(a)| |c|  = |c*cos(a) - r*sin(a)|  = |c'|
    |sin(a)  cos(a)| |r|    |c*sin(a) + r*cos(a)|  = |r'|
    */
    // -90-degree rotation
    int r, c;
    int new_w = h, new_h = w;
    int new_r, new_c, pix, new_pix;
    int max = w * h * 3;
    for (r = 0; r < h; r++) {
        for (c = 0; c < w; c++) {
            pix = (r * w + c) * 3;
            new_c = 0 + r;
            new_r = -c + 0 + (w - 1);
            new_pix = (new_r * new_w + new_c) * 3;
            dst[new_pix + 0] = src[pix + 0];
            dst[new_pix + 1] = src[pix + 1];
            dst[new_pix + 2] = src[pix + 2];
        }
    }
}