#ifndef OPENCL_EXCLUSIVE_SCAN
#define OPENCL_EXCLUSIVE_SCAN

#define CL_TARGET_OPENCL_VERSION 120
#include <CL/cl.h>

#define MAX_SOURCE_SIZE 100000

static char* value;
static size_t valueSize;
static cl_uint maxComputeUnits;
static cl_platform_id platform_id[1];
static cl_device_id device_id[1];   
static cl_uint ret_num_devices;
static cl_uint ret_num_platforms;
static cl_int ret;
static cl_context context;
static cl_command_queue command_queue;
static cl_mem in_mem;
static cl_mem out_mem;
static cl_mem jump_mem;
static cl_mem length;
static cl_program program;
static cl_kernel kernel, kernel_copy;
static bool once = false;

template<class IN_TYPE, class OUT_TYPE>
bool exclusive_scan(const IN_TYPE* in, OUT_TYPE* out, int size) {
    // TODO make class
    if (!once) {
        once = true;
        ret = clGetPlatformIDs(1, platform_id, &ret_num_platforms);
        for (int i = 0; i < ret_num_platforms; i++) {
                ret = clGetDeviceIDs( platform_id[i], CL_DEVICE_TYPE_DEFAULT, 1, 
                        device_id, &ret_num_devices);

                // for each device print critical attributes
                for (int j = 0; j < ret_num_devices; j++) {

                // print device name
                clGetDeviceInfo(device_id[j], CL_DEVICE_NAME, 0, NULL, &valueSize);
                value = (char*) malloc(valueSize);
                clGetDeviceInfo(device_id[j], CL_DEVICE_NAME, valueSize, value, NULL);
                printf("%d. Device: %s\n", j+1, value);
                free(value);

                clGetDeviceInfo(device_id[j], CL_DEVICE_VERSION, 0, NULL, &valueSize);
                value = (char*) malloc(valueSize);
                clGetDeviceInfo(device_id[j], CL_DEVICE_VERSION, valueSize, value, NULL);
                printf(" %d.%d Hardware version: %s\n", j+1, 1, value);
                free(value);

                clGetDeviceInfo(device_id[j], CL_DRIVER_VERSION, 0, NULL, &valueSize);
                value = (char*) malloc(valueSize);
                clGetDeviceInfo(device_id[j], CL_DRIVER_VERSION, valueSize, value, NULL);
                printf(" %d.%d Software version: %s\n", j+1, 2, value);
                free(value);

                clGetDeviceInfo(device_id[j], CL_DEVICE_OPENCL_C_VERSION, 0, NULL, &valueSize);
                value = (char*) malloc(valueSize);
                clGetDeviceInfo(device_id[j], CL_DEVICE_OPENCL_C_VERSION, valueSize, value, NULL);
                printf(" %d.%d OpenCL C version: %s\n", j+1, 3, value);
                free(value);

                clGetDeviceInfo(device_id[j], CL_DEVICE_MAX_COMPUTE_UNITS,
                        sizeof(maxComputeUnits), &maxComputeUnits, NULL);
                printf(" %d.%d Parallel compute units: %d\n", j+1, 4, maxComputeUnits);
                }
        }

        context = clCreateContext( NULL, 1, device_id, NULL, NULL, &ret);
        command_queue = clCreateCommandQueue(context, device_id[0], 0, &ret);
        
        // Create memory buffers on the device for each vector 
        in_mem = clCreateBuffer(context, CL_MEM_READ_ONLY, 
                32 * 1024 * 1024 * sizeof(int64_t), NULL, &ret);
        out_mem = clCreateBuffer(context, CL_MEM_READ_WRITE,
                32 * 1024 * 1024 * sizeof(int64_t), NULL, &ret);
        jump_mem = clCreateBuffer(context, CL_MEM_READ_ONLY, 
                sizeof(int), NULL, &ret);
        length = clCreateBuffer(context, CL_MEM_READ_ONLY, 
                sizeof(int), NULL, &ret);

        FILE *fp;
        char *source_str;
        size_t source_size; 
        fp = fopen("src/opencl_exclusive_scan.cl", "r");
        if (!fp) {
                fprintf(stderr, "Failed to load kernel.\n");
                exit(1);
        }
        source_str = (char*)malloc(MAX_SOURCE_SIZE);
        source_size = fread( source_str, 1, MAX_SOURCE_SIZE, fp);
        fclose( fp );

        program = clCreateProgramWithSource(context, 1, 
                (const char **)&source_str, (const size_t *)&source_size, &ret);
        
        ret = clBuildProgram(program, 1, device_id, NULL, NULL, NULL);
        assert(ret == CL_SUCCESS);
        auto kernel_name_copy = std::string("exclusive_copy_") + std::to_string(sizeof(IN_TYPE)) + "_" + std::to_string(sizeof(OUT_TYPE));
        auto kernel_name = std::string("exclusive_scan_") + std::to_string(sizeof(IN_TYPE)) + "_" + std::to_string(sizeof(OUT_TYPE));
        std::cout << kernel_name << std::endl;
        kernel_copy = clCreateKernel(program, kernel_name_copy.data(), &ret);
        assert(ret == CL_SUCCESS);
        ret = clSetKernelArg(kernel_copy, 0, sizeof(cl_mem), (void *)&in_mem);
        assert(ret == CL_SUCCESS);
        ret = clSetKernelArg(kernel_copy, 1, sizeof(cl_mem), (void *)&out_mem);
        assert(ret == CL_SUCCESS);
        ret = clSetKernelArg(kernel_copy, 2, sizeof(cl_mem), (void *)&jump_mem);
        assert(ret == CL_SUCCESS);
        ret = clSetKernelArg(kernel_copy, 3, sizeof(cl_mem), (void *)&length);
        assert(ret == CL_SUCCESS);
        kernel = clCreateKernel(program, kernel_name.data(), &ret);
        assert(ret == CL_SUCCESS);
        ret = clSetKernelArg(kernel, 0, sizeof(cl_mem), (void *)&in_mem);
        assert(ret == CL_SUCCESS);
        ret = clSetKernelArg(kernel, 1, sizeof(cl_mem), (void *)&out_mem);
        assert(ret == CL_SUCCESS);
        ret = clSetKernelArg(kernel, 2, sizeof(cl_mem), (void *)&jump_mem);
        assert(ret == CL_SUCCESS);
        ret = clSetKernelArg(kernel, 3, sizeof(cl_mem), (void *)&length);
        assert(ret == CL_SUCCESS);
    }

    ret = clEnqueueWriteBuffer(command_queue, in_mem, CL_FALSE, 0,
            size * sizeof(IN_TYPE), in, 0, NULL, NULL);
    assert(ret == CL_SUCCESS);
    ret = clEnqueueWriteBuffer(command_queue, length, CL_FALSE, 0,
            sizeof(int), &size, 0, NULL, NULL);
    assert(ret == CL_SUCCESS);

    size_t local_item_size = 960;
    size_t global_item_size = local_item_size * ((local_item_size - 1 + size) / local_item_size);
    ret = clEnqueueNDRangeKernel(command_queue, kernel_copy, 1, NULL, 
        &global_item_size, &local_item_size, 0, NULL, NULL);
    assert(ret == CL_SUCCESS);
    
    for (int jump = 1; jump < size; jump *= 2) {
        ret = clEnqueueWriteBuffer(command_queue, jump_mem, CL_FALSE, 0,
                sizeof(int), &jump, 0, NULL, NULL);
        assert(ret == CL_SUCCESS);
        ret = clEnqueueNDRangeKernel(command_queue, kernel, 1, NULL, 
                &global_item_size, &local_item_size, 0, NULL, NULL);
        assert(ret == CL_SUCCESS);
    }
    ret = clEnqueueReadBuffer(command_queue, out_mem, CL_TRUE, 0, 
            size * sizeof(OUT_TYPE), out, 0, NULL, NULL);
    assert(ret == CL_SUCCESS);

    return true;
}

#endif // OPENCL_EXCLUSIVE_SCAN