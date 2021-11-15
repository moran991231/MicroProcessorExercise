// OpenCL kernel. Each work item takes care of one element of c

// #define __kernel
// #define __global
#pragma OPENCL EXTENSION cl_khr_fp64 : enable

__kernel void kernel_gray(__global unsigned char *src, __global unsigned char *dst, const int w,
                          const int h) {
    int pix = get_global_id(0) * 3;

    float gray;
    gray = src[pix + 0] * 0.2126f;
    gray += src[pix + 1] * 0.7152f;
    gray += src[pix + 2] * 0.0722f;
    dst[pix + 0] = dst[pix + 1] = dst[pix + 2] = gray;
}

__kernel void kernel_rotate(__global unsigned char *src, __global unsigned char *dst, const int w,
                            const int h) {
    int r = get_global_id(0) / w;
    int c = get_global_id(0) % w;
    int new_w = h, new_h = w;
    int new_r, new_c, pix, new_pix;

    pix = (r * w + c) * 3;
    new_c = 0 + r;
    new_r = -c + 0 + (w - 1);
    new_pix = (new_r * new_w + new_c) * 3;
    dst[new_pix + 0] = src[pix + 0];
    dst[new_pix + 1] = src[pix + 1];
    dst[new_pix + 2] = src[pix + 2];
}
