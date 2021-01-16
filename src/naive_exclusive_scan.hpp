#ifndef NAIVE_EXCLUSIVE_SCAN
#define NAIVE_EXCLUSIVE_SCAN

template<class IN_TYPE, class OUT_TYPE>
bool exclusive_scan(const IN_TYPE* in, OUT_TYPE* out, int size) {
    out[0] = OUT_TYPE(0);
    for (int i = 0, n = size - 1; i < n; ++i) {
        out[i + 1] = out[i] + OUT_TYPE(in[i]);
    }
    return true;
}

#endif // NAIVE_EXCLUSIVE_SCAN