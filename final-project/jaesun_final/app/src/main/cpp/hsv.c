// OpenCL kernel. Each work item takes care of one element of c

#pragma OPENCL EXTENSION cl_khr_fp64 : enable

__kernel void kernel_rgb2hsv(__global unsigned char *src, __global unsigned char *dst, const int w,
                             const int h) {
    float hue, sat, max_min;
    int r, g, b; // 255
    int max, min; //255
    int pix = ((get_global_id(0) / w) * w + get_global_id(0) % w) * 4;

    r = (int) src[pix + 0] & 0x000000FF;
    g = (int) src[pix + 1] & 0x000000FF;
    b = (int) src[pix + 2] & 0x000000FF;
    max = min = r;
    max = (max > g) ? max : g;
    max = (max > b) ? max : b;
    min = (min < g) ? min : g;
    min = (min < b) ? min : b;


    if (max == 0)
        hue = sat = 0.0f;
    else {
        max_min = max - min;
        sat = max_min / max; // 1.0
        if (max == r) {
            hue = 60.0f * (g - b) / max_min; // 60.0
        } else if (max == g) {
            hue = 120.0f + 60.0f * ((b - r) / max_min); // 180.0
        } else {
            hue = 240.0f + 60.0f * ((r - g) / max_min); // 300.0
        }
    }
    r = (int) (hue / 360.0f * 255.0f);
    if (r < 0) r += 255;
    else if (r > 255) r -= 255;

    g = (int) (sat * 255.0f);
    if (g < 0) g += 255;
    else if (g > 255) g -= 255;

    b = max;

    dst[pix + 0] = (unsigned char) (r & 0x000000FF);
    dst[pix + 1] = (unsigned char) (g & 0x000000FF);
    dst[pix + 2] = (unsigned char) (b & 0x000000FF);
}
