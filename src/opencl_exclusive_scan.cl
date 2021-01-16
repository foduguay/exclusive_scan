__kernel void exclusive_copy_1_1(__global const char *in_mem, __global char *out_mem, __global const int *jump, __global const int *length) {
    int i = get_global_id(0);
    out_mem[0] = 0;
    if (1 <= i && i < *length) {
        out_mem[i] = in_mem[i - 1];
    }
}

__kernel void exclusive_scan_1_1(__global const char *in_mem, __global char *out_mem, __global const int *jump, __global const int *length) {
    int i = get_global_id(0);
    if (*jump <= i && i < *length) {
        out_mem[i] += out_mem[i - *jump];
    }
}

__kernel void exclusive_scan_4_4(__global const int *in_mem, __global int *out_mem, __global const int *jump, __global const int *length) {
    int i = get_global_id(0);
    if (0 <= i - *jump && i < *length) {
        out_mem[i] += in_mem[i - *jump];
    }
}

__kernel void exclusive_scan_8_8(__global const long *in_mem, __global long *out_mem, __global const int *jump, __global const int *length) {
    int i = get_global_id(0);
    if (0 <= i - *jump && i < *length) {
        out_mem[i] += in_mem[i - *jump];
    }
}