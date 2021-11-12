#ifndef __IMAGE_PROCESSING_H__
#define __IMAGE_PROCESSING_H__
typedef struct __attribute__((__packed__)) BMPHeader {
    char bfType[2];
    int bfSize, bfReserved, bfOffBits, biSize, biWidth, biHeight;
    short biPlanes, biBitCount;
    int biCompression, biSizeImage, biXpelsPerMeters, biYPelsPerMeter, biClrUsed, biClrImportant;
} BMPHEADER;
#endif