#include <stdio.h>
#include <stdlib.h>

#if __APPLE__
#include <OpenCL/opencl.h>
#else
#include <CL/cl.h>
#endif

#define MAX_SOURCE_SIZE (0x100000)

int main(int argc, char* args[]) {
    const int LEN = 32;
    int *a = (int*)malloc(sizeof(int) * LEN);
    int *b = (int*)malloc(sizeof(int) * LEN);
    int *c = (int*)malloc(sizeof(int) * LEN);

    // Load the kernel source into src_str
    FILE* src_file;
    char* src_str;
    size_t src_size;

    src_file = fopen("devcode/plus_vector.cl", "r");
    if(!src_file) {
        fprintf(stderr, "Failed to load kernel code");
        free(a);
        free(b);
        free(c);
        exit(1);
    }

    src_str = (char*)malloc(MAX_SOURCE_SIZE);
    src_size = fread(src_str, 1, MAX_SOURCE_SIZE, src_file);
    fclose(src_file);

    // Get platform and device information
    cl_platform_id platform_id = NULL;
    cl_device_id dev_id = NULL;
    cl_uint num_devices;
    cl_uint num_platforms;
    cl_int ret = clGetPlatformIDs(1, &platform_id, &num_platforms);
    if(ret == CL_SUCCESS) {
        printf("clGetPlatformIDs is OK \n");
        cl_int ret_dev = clGetDeviceIDs(platform_id, CL_DEVICE_TYPE_DEFAULT, 1, &dev_id, &num_devices);
        if(ret_dev != CL_SUCCESS) {
            switch(ret_dev){
                case CL_INVALID_PLATFORM:
                    printf("CL_INVALID_PLATFORM \n");
                    break;
                case CL_INVALID_DEVICE_TYPE:
                    printf("CL_INVALID_DEVICE_TYPE \n");
                    break;
                case CL_INVALID_VALUE:
                    printf("CL_INVALID_VALUE \n");
                    break;
                case CL_DEVICE_NOT_FOUND:
                    printf("CL_DEVICE_NOT_FOUND \n");
                    break;
                default:
                    printf("clGetDeviceIDs returns: %d \n", ret_dev);
            }
            free(src_str);
            free(a);
            free(b);
            free(c);
            exit(1);
        }

        // Get CL context
        cl_context ctx = clCreateContext(NULL, 1, &dev_id, NULL, NULL, &ret);

        // Create a command queue
        cl_command_queue cmd_queue = clCreateCommandQueue(ctx, dev_id, 0, &ret);

        // Create memory buffer for each vector
        cl_mem mem_a = clCreateBuffer(ctx, CL_MEM_READ_ONLY, LEN * sizeof(int), NULL, &ret);
        cl_mem mem_b = clCreateBuffer(ctx, CL_MEM_READ_ONLY, LEN * sizeof(int), NULL, &ret);
        cl_mem mem_c = clCreateBuffer(ctx, CL_MEM_WRITE_ONLY, LEN * sizeof(int), NULL, &ret);

        // Copy the two lists a and b two their buffers respectively
        ret = clEnqueueWriteBuffer(cmd_queue, mem_a, CL_TRUE, 0, LEN * sizeof(int), a, 0, NULL, NULL);
        ret - clEnqueueWriteBuffer(cmd_queue, mem_b, CL_TRUE, 0, LEN * sizeof(int), b, 0, NULL, NULL);

        // Create a kernel program from source
        cl_program program = clCreateProgramWithSource(ctx, 1, (const char **)&src_str, (const size_t *)&src_size, &ret);

        // Build the program
        ret = clBuildProgram(program, 1, &dev_id, NULL, NULL, NULL);

        // Create OpenCL kernel
        cl_kernel kernel = clCreateKernel(program, "vector_add", &ret);

        // Set the argument of the kernel
        ret = clSetKernelArg(kernel, 0, sizeof(cl_mem), (void*)&mem_a);
        ret = clSetKernelArg(kernel, 0, sizeof(cl_mem), (void*)&mem_b);
        ret = clSetKernelArg(kernel, 0, sizeof(cl_mem), (void*)&mem_c);

        // Execute the opencl kernel on the list
        size_t global_item_size = LEN; //process the entire list
        size_t local_item_size = 16;
        ret = clEnqueueNDRangeKernel(cmd_queue, kernel, 1, NULL, &global_item_size, 
            &local_item_size, 0, NULL, NULL);

        // Clear up
    }
    
    free(src_str);
    free(a);
    free(b);
    free(c);
    return 0;
}
