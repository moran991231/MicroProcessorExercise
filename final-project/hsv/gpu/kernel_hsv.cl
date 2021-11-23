// OpenCL kernel. Each work item takes care of one element of c

// #define __kernel
// #define __global
#pragma OPENCL EXTENSION cl_khr_fp64 : enable

__kernel void kernel_rgb2hsv(__global unsigned char *src, __global unsigned char *dst, const int w,
                             const int h) {
    double hue, sat, val, max_min;
    double r, g, b;
    double max, min;
    int pix = ((get_global_id(0) / w) * w + get_global_id(0) % w) * 3;

    r = (double)src[pix + 0] / 255.0;
    g = (double)src[pix + 1] / 255.0;
    b = (double)src[pix + 2] / 255.0;
    max = min = r;
    max = (max > g) ? max : g;
    max = (max > b) ? max : b;
    min = (min < g) ? min : g;
    min = (min < b) ? min : b;

    val = max;
    if (val == 0)
        hue = sat = 0.0;
    else {
        sat = (val - min) / val;
        if (val == r) {
            hue = 60.0 * (g - b) / (max - min);
        } else if (val == g) {
            hue = 120.0 + 60.0 * ((b - r) / (max - min));
        } else {
            hue = 240.0 + 60.0 * ((r - g) / (max - min));
        }
    }
    if (hue < 0)
        hue += 360.0;
    else if (hue > 360)
        hue -= 360.0;

    sat = (sat >= 1) ? 1 : sat;
    sat = (sat <= 0) ? 0 : sat;

    dst[pix + 0] = (unsigned char)((hue / 360.0) * 255.0);
    dst[pix + 1] = (unsigned char)(sat * 255.0);
    dst[pix + 2] = (unsigned char)(val * 255.0);
}
