# exclusive_scan
An exclusive scan takes as input an array of size n and output of the same size.

# to run the bench test
'git clone https://github.com/foduguay/exclusive_scan.git'
'make'
'./exclusive_scan.out'

/home/fod/git/exclusive_scan/data/test/test_fail_2
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
TEST /home/fod/git/exclusive_scan/data/test/test_success_1 PASSED!

          time in milliseconds for computations
          -------------------------------------
block size|     naive |    opencl |   threads |
          -------------------------------------
     1024 |   2771.27 |   1383.79 |   1208.11 |
     4096 |    670.36 |    319.02 |    302.13 |
     8192 |    331.92 |    165.67 |    151.40 |
    16384 |    173.70 |     79.93 |     76.44 |
    32768 |     75.95 |     40.15 |     38.14 |
    65536 |     32.08 |     20.08 |     19.96 |
   131072 |     16.42 |     10.31 |     13.56 |
   262144 |      7.76 |      5.19 |      5.50 |
   524288 |      3.81 |      2.64 |      2.72 |
  1048576 |      2.01 |      1.36 |      1.36 |
          -------------------------------------
