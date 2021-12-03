// OpenCL kernel. Each work item takes care of one element of c

#pragma OPENCL EXTENSION cl_khr_fp64 : enable

__kernel void kernel_in_range_(__global unsigned char *src, __global unsigned char *dst, const int w,
                             const int h, __global unsigned char* range) {
    int pix = ((get_global_id(0) / w) * w + get_global_id(0) % w) * 4;
    unsigned char flag=255;
    unsigned char down, up, val;
    int i=0;
    for(i=0; i<3; i++){
        val = src[pix+i];
        down=range[i*2]; up=range[i*2+1];
        if(down<=up){
            if(!((down<=val) && (val<=up))) flag=0;
        }else{

            if(!(down<=val || val<=up)) flag=0;
        }
        if(flag==0) break;
    }
    dst[pix+0] = dst[pix+1] = dst[pix+2] = flag;
}



