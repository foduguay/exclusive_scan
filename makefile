CC=g++
CFLAGS=-I. -pthread -std=c++2a src/main.cpp -I/usr/local/cuda/include -L/usr/lib/nvidia-current -lOpenCL

exclusive_scan:
	$(CC) $(CFLAGS) -o exclusive_scan.out

clean:
	rm -f *.o exclusive_scan.out

