#ifndef EXCLUSIVE_SCAN
#define EXCLUSIVE_SCAN

#include <vector>

namespace naive_exclusive_scan {
    #include "naive_exclusive_scan.hpp"
};
namespace avx_exclusive_scan {
    #include "avx_exclusive_scan.hpp"
};
namespace cuda_exclusive_scan {
    #include "cuda_exclusive_scan.hpp"
};

#endif // EXCLUSIVE_SCAN