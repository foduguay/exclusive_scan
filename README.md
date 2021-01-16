# exclusive_scan
An exclusive scan takes as input an array of size n and output of the same size.

# to run the bench test
### git clone https://github.com/foduguay/exclusive_scan.git
### make
### ./exclusive_scan.out

'/home/fod/git/exclusive_scan/data/test/test_fail_2
RUNNING /home/fod/git/exclusive_scan/data/test/test_fail_2
__char__
<=/home/fod/git/exclusive_scan/data/test/test_fail_2.in: read 1024 numbers
<=/home/fod/git/exclusive_scan/data/test/test_fail_2.exp: read 1024 numbers
1. Device: GeForce GTX 1080 Ti
 1.1 Hardware version: OpenCL 1.2 CUDA
 1.2 Software version: 460.27.04
 1.3 OpenCL C version: OpenCL C 1.2 
 1.4 Parallel compute units: 28
exclusive_scan_1_1
=>/home/fod/git/exclusive_scan/data/test/test_fail_2.out: wrote 1024 numbers
TEST /home/fod/git/exclusive_scan/data/test/test_fail_2 PASSED!

/home/fod/git/exclusive_scan/data/test/test_fail_1
RUNNING /home/fod/git/exclusive_scan/data/test/test_fail_1
__char__
<=/home/fod/git/exclusive_scan/data/test/test_fail_1.in: read 1024 numbers
<=/home/fod/git/exclusive_scan/data/test/test_fail_1.exp: read 1024 numbers
=>/home/fod/git/exclusive_scan/data/test/test_fail_1.out: wrote 1024 numbers
TEST /home/fod/git/exclusive_scan/data/test/test_fail_1 PASSED!

/home/fod/git/exclusive_scan/data/test/test_success_2
RUNNING /home/fod/git/exclusive_scan/data/test/test_success_2
__char__
<=/home/fod/git/exclusive_scan/data/test/test_success_2.in: read 1024 numbers
<=/home/fod/git/exclusive_scan/data/test/test_success_2.exp: read 1024 numbers
=>/home/fod/git/exclusive_scan/data/test/test_success_2.out: wrote 1024 numbers
TEST /home/fod/git/exclusive_scan/data/test/test_success_2 PASSED!

/home/fod/git/exclusive_scan/data/test/test_success_1
RUNNING /home/fod/git/exclusive_scan/data/test/test_success_1
__char__
<=/home/fod/git/exclusive_scan/data/test/test_success_1.in: read 1024 numbers
<=/home/fod/git/exclusive_scan/data/test/test_success_1.exp: read 1024 numbers
=>/home/fod/git/exclusive_scan/data/test/test_success_1.out: wrote 1024 numbers
TEST /home/fod/git/exclusive_scan/data/test/test_success_1 PASSED!'

|----------------------------------------------|
|          |computation time in milliseconds   |
|----------|-----------------------------------|
|block size|     naive |    opencl |   threads |
|----------|-----------------------------------|
|     1024 |   1892.05 |   1159.16 |   1200.15 |
|     4096 |    507.18 |    271.71 |    305.90 |
|     8192 |    240.38 |    138.02 |    147.92 |
|    16384 |    133.67 |     69.22 |     76.33 |
|    32768 |     60.09 |     34.60 |     39.80 |
|    65536 |     31.51 |     17.39 |     21.18 |
|   131072 |     15.01 |      8.76 |     10.95 |
|   262144 |      8.00 |      4.36 |      5.51 |
|   524288 |      3.88 |      2.24 |      2.66 |
|  1048576 |      1.90 |      1.14 |      1.37 |
|----------|-----------|-----------|-----------|


|-----------|
|Future work|
|-----------|

I need to profile this code, I'm sure we could get a lot of performance gain just by avoiding copies here and there. The naive algorithm has a linear runtime and linear memory footprint. The OpenCL algorithm is a logarithmic strategy where the memory location twice the distance is added at every step. Since every memory location is populated, the algorithm has a runtime of n * lg(n) but runs in parallel, and linear in memory footprint. For the threaded solution, the idea is to devide the problem into 20 sub problems and then stitch the extremities. Runtime for the threaded solution is 2 * (n / 20) + 20 and uses linear memory footprint.

The OpenCL code is messy right now, I meant to put the initialization in a class, but fell short on time.
I also wanted to exploit the fact that we are dealing with bytes to use AVX operations for fast sums. The plan was to have different implementations for different data size, support for char, short, int, long, float and double. CUDA would probably have done a better job than OpenCL, but I started using C++2a, which seems to be incompatible with the version of nvcc I was using. I probably should have stuck with C++0x.

I've also had many issues with Vectors fragmenting memory which was causing many headaches. I moved back to Arrays last minutes because I was unable to get the system working correctly with Vectors.
