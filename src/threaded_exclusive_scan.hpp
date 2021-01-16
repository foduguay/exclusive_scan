#ifndef THREADED_EXCLUSIVE_SCAN
#define THREADED_EXCLUSIVE_SCAN

#include <iostream>
#include <string.h>

void partial_exclusive_scan(char* in, char* out, int size, int part, int offset) {
    for (int i = std::max(0, offset), n = std::min(size, offset + part); i < n; ++i) {
        out[i + 1] = out[i] + in[i];
    }
};

void add_other_parts(char* in, char* out, int size, int part, int offset) {
    int start = std::max(0, offset);
    for (int i = start, n = std::min(size, offset + part); i < n; ++i) {
        out[i + 1] += out[start];
    }
};

bool exclusive_scan(char* in, char* out, int size) {
    memset(out, 0, size * sizeof(char));
    int thread_count = 20;
    int offset = (size + thread_count - 1) / thread_count;
    std::thread t[thread_count];
    for (int i=0; i<thread_count; ++i) {
        t[i] = std::thread(partial_exclusive_scan, in, out, size, offset - 1, offset * i);
    }
    for (int i=0; i<thread_count; ++i) {
        t[i].join();
    }
    for (int i = offset; i < size; i += offset) {
        out[i] += out[i - offset] + out[i - 1] + in[i - 1];
    }
    for (int i=0; i<thread_count; ++i) {
        t[i] = std::thread(add_other_parts, in, out, size, offset - 1, offset * i);
    }
    for (int i=0; i<thread_count; ++i) {
        t[i].join();
    }
    return true;
}

#endif // THREADED_EXCLUSIVE_SCAN
