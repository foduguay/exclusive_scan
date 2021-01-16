//static __global__ void kernel(const float *A, const float *b) {
//}

template<class IN_TYPE, class OUT_TYPE>
bool exclusive_scan(const IN_TYPE* in, OUT_TYPE* out, int size) {
    /*float *d_a, *d_b;
    if(cudaMalloc(&d_a, sizeof(float)) != cudaSuccess) {
        std::cout << "cudaMalloc d_a failed" << std::endl;
	return 1;
    }
    if(cudaMalloc(&d_b, sizeof(float)) != cudaSuccess) {
        std::cout << "cudaMalloc d_b failed" << std::endl;
	cudaFree(d_a);
	return 1;
    }
    kernel<<<1, 1>>>(d_a, d_b);
*/
    out[0] = OUT_TYPE(0);
    for (int i = 0, n = size - 1; i < n; ++i) {
        out[i + 1] = out[i] + OUT_TYPE(in[i]);
    }
    return true;
}
