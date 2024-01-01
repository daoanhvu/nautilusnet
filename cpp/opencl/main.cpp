#include <stdlib.h>
#include <stdio.h>
#include <iostream>

#define CL_USE_DEPRECATED_OPENCL_1_2_APIS
#ifdef __APPLE__
  #include <OpenCL/opencl.h>
#else
  #include <CL/cl.h>
#endif

#define MAX_NUM_DEVICE 8
#define MAX_DEV_NAME 1024
#define TS 8       // The square-root of the 2D tile-size (== work-group dims)

using namespace std;

void checkError(cl_int error, int line);
void mul(int n, int m, int r, const float *a, const float *b, float *c);
char* readKernelFile(const char* filename, long* size);
void printMatrix(int row, int col, const float* m);

int main(int argc, char **argv) {
  int M = 2;
  int K = 4;
  int N = 6;
  float A[8] = {
    1, 4, 2, 3,
    5, 2, 1, 7
  };

  float B[24] = {
    5, 9, 2, 1, 0, 6,
    1, 2, 9, 2, 1, 1,
    4, 3, 1, 9, 8, 9,
    4, 5, 2, 1, 3, 5
  };

  float C[12];

  mul(M, N, K, A, B, C);

  printMatrix(M, N, C);

  return 0;
}

void mul(int M, int N, int K, const float *A, const float *B, float *C) {
  cl_mem da;
  cl_mem db;
  cl_mem dc;

  cl_int ret;
  cl_uint numDevice = 0;
  cl_platform_id platform = 0;
  cl_context_properties props[3] = {CL_CONTEXT_PLATFORM, 0, 0};
  cl_context ctx = 0;
  cl_command_queue cmd_queue = 0;
  cl_program program = NULL;
  cl_event event = NULL;
  cl_device_id device = 0;
  cl_device_id devices[MAX_NUM_DEVICE];
  char deviceName[MAX_DEV_NAME];
  const int used_device_idx = 0;
  size_t logSize;
  char *message;

  ret = clGetPlatformIDs(1, &platform, NULL);

  // Call clGetDeveiceIDs for the first time to get number of device
  ret = clGetDeviceIDs( 
      platform, 
      CL_DEVICE_TYPE_DEFAULT, 
      0, 
      NULL, 
      &numDevice);

  // Call clGetDeviceIDs for the second time to get list of devices
  ret = clGetDeviceIDs( 
      platform, 
      CL_DEVICE_TYPE_DEFAULT, 
      numDevice, 
      devices, 
      NULL);

  
  device = devices[used_device_idx];
  props[1] = (cl_context_properties)platform;
  ctx = clCreateContext(props, numDevice, &device, NULL, NULL, &ret);
  // We should use clCreateCommandQueueWithProperties()
  cmd_queue = clCreateCommandQueue(ctx, device, 0, &ret);
  ret = clGetDeviceInfo(device, CL_DEVICE_NAME, MAX_DEV_NAME, deviceName, NULL);
  // printf("## %d devices, running on %d: '%s'\n", numDevice, used_device_idx, deviceName);
  
  // Read kernel file from file
  long srcSize;
  char* _src = readKernelFile("kernels/gemm.cl", &srcSize);
  const char* src = _src; 
  program = clCreateProgramWithSource(ctx, 1, &src, NULL, &ret);

  const char *compOpt = "";
  clBuildProgram(program, 0, NULL, compOpt, NULL, NULL);
  ret = clGetProgramBuildInfo(program, device, CL_PROGRAM_BUILD_LOG, 0, NULL, &logSize);
  message = (char*)malloc((logSize+1)*sizeof(char));
  ret = clGetProgramBuildInfo(program, device, CL_PROGRAM_BUILD_LOG, logSize, message, NULL);
  message[logSize] = '\0';
  std::cout << "logSize: " << logSize << std::endl;
  if (logSize > 10) { printf("## Compiler message: %s\n", message); }
  free(message);

  // Retrieve the PTX code from OpenCL and write it to a binary file
  size_t binSize;
  ret = clGetProgramInfo(program, CL_PROGRAM_BINARY_SIZES, sizeof(size_t), &binSize, NULL);
  unsigned char *bin = (unsigned char *)malloc(binSize);
  ret = clGetProgramInfo(program, CL_PROGRAM_BINARIES, sizeof(unsigned char), &bin, NULL);
  FILE *binF = fopen("/home/ddao/data/ptx_opencl.bin", "wb");
  if(binF == NULL) {
    std::cout << "Cannot open ptx_opencl.bin" << std::endl;
  }
  fwrite(bin, sizeof(char), binSize, binF);
  fclose(binF);
  free(bin);

  da = clCreateBuffer(ctx, CL_MEM_READ_ONLY, M*K*sizeof(*A), NULL, &ret);
  db = clCreateBuffer(ctx, CL_MEM_READ_ONLY, K*N*sizeof(*B), NULL, &ret);
  dc = clCreateBuffer(ctx, CL_MEM_READ_WRITE, M*N*sizeof(*C), NULL, &ret);

  ret = clEnqueueWriteBuffer(cmd_queue, da, CL_TRUE, 0, M*K*sizeof(*A), A, 0, NULL, NULL);
  ret = clEnqueueWriteBuffer(cmd_queue, db, CL_TRUE, 0, K*N*sizeof(*B), B, 0, NULL, NULL);
  ret = clEnqueueWriteBuffer(cmd_queue, dc, CL_TRUE, 0, M*N*sizeof(*C), C, 0, NULL, NULL);

  // Configure the kernel
  char kernelName[64];
  sprintf(kernelName, "gemm1");
  cl_kernel gemmKernel = clCreateKernel(program, kernelName, &ret);

  ret = clSetKernelArg(gemmKernel, 0, sizeof(int), (void*)&M);
  ret = clSetKernelArg(gemmKernel, 1, sizeof(int), (void*)&N);
  ret = clSetKernelArg(gemmKernel, 2, sizeof(int), (void*)&K);
  ret = clSetKernelArg(gemmKernel, 3, sizeof(cl_mem), (void*)&da);
  ret = clSetKernelArg(gemmKernel, 4, sizeof(cl_mem), (void*)&db);
  ret = clSetKernelArg(gemmKernel, 5, sizeof(cl_mem), (void*)&dc);
  // Configure the thread per work-item
  const size_t local[2] = { TS, TS };
  const size_t global[2] = { static_cast<size_t>(M), static_cast<size_t>(N) };
  ret = clEnqueueNDRangeKernel(cmd_queue, gemmKernel, 
    2, NULL, global, local, 0, NULL, &event);
  ret = clWaitForEvents(1, &event);

  // Copy the output matrix back to host
  ret = clEnqueueReadBuffer(cmd_queue, dc, CL_TRUE, 0, M*N*sizeof(*C), C, 0, NULL, NULL);

  free(_src);
  // Clean-up OpenCL
  clReleaseMemObject(da);
  clReleaseMemObject(db);
  clReleaseMemObject(dc);
  clReleaseCommandQueue(cmd_queue);
  clReleaseContext(ctx);
  clReleaseProgram(program);
  clReleaseKernel(gemmKernel);
}

char* readKernelFile(const char* filename, long* size) {
  FILE *file = fopen(filename, "r");
  if(!file) {
    return 0;
  }

  // Get file size
  fseek(file, 0, SEEK_END);
  long sz = ftell(file);
  rewind(file);

  // Read text from the given file
  char *src = (char*)malloc((sz+1) * sizeof(char));
  fread(src, 1, sz * sizeof(char), file);
  src[sz] = '\0';
  fclose(file);

  *size = sz + 1;
  return src;
}

// Print an error message to screen (only if it occurs)
void checkError(cl_int error, int line) {
    if (error != CL_SUCCESS) {
        switch (error) {
            case CL_DEVICE_NOT_FOUND:                 printf("-- Error at %d:  Device not found.\n", line); break;
            case CL_DEVICE_NOT_AVAILABLE:             printf("-- Error at %d:  Device not available\n", line); break;
            case CL_COMPILER_NOT_AVAILABLE:           printf("-- Error at %d:  Compiler not available\n", line); break;
            case CL_MEM_OBJECT_ALLOCATION_FAILURE:    printf("-- Error at %d:  Memory object allocation failure\n", line); break;
            case CL_OUT_OF_RESOURCES:                 printf("-- Error at %d:  Out of resources\n", line); break;
            case CL_OUT_OF_HOST_MEMORY:               printf("-- Error at %d:  Out of host memory\n", line); break;
            case CL_PROFILING_INFO_NOT_AVAILABLE:     printf("-- Error at %d:  Profiling information not available\n", line); break;
            case CL_MEM_COPY_OVERLAP:                 printf("-- Error at %d:  Memory copy overlap\n", line); break;
            case CL_IMAGE_FORMAT_MISMATCH:            printf("-- Error at %d:  Image format mismatch\n", line); break;
            case CL_IMAGE_FORMAT_NOT_SUPPORTED:       printf("-- Error at %d:  Image format not supported\n", line); break;
            case CL_BUILD_PROGRAM_FAILURE:            printf("-- Error at %d:  Program build failure\n", line); break;
            case CL_MAP_FAILURE:                      printf("-- Error at %d:  Map failure\n", line); break;
            case CL_INVALID_VALUE:                    printf("-- Error at %d:  Invalid value\n", line); break;
            case CL_INVALID_DEVICE_TYPE:              printf("-- Error at %d:  Invalid device type\n", line); break;
            case CL_INVALID_PLATFORM:                 printf("-- Error at %d:  Invalid platform\n", line); break;
            case CL_INVALID_DEVICE:                   printf("-- Error at %d:  Invalid device\n", line); break;
            case CL_INVALID_CONTEXT:                  printf("-- Error at %d:  Invalid context\n", line); break;
            case CL_INVALID_QUEUE_PROPERTIES:         printf("-- Error at %d:  Invalid queue properties\n", line); break;
            case CL_INVALID_COMMAND_QUEUE:            printf("-- Error at %d:  Invalid command queue\n", line); break;
            case CL_INVALID_HOST_PTR:                 printf("-- Error at %d:  Invalid host pointer\n", line); break;
            case CL_INVALID_MEM_OBJECT:               printf("-- Error at %d:  Invalid memory object\n", line); break;
            case CL_INVALID_IMAGE_FORMAT_DESCRIPTOR:  printf("-- Error at %d:  Invalid image format descriptor\n", line); break;
            case CL_INVALID_IMAGE_SIZE:               printf("-- Error at %d:  Invalid image size\n", line); break;
            case CL_INVALID_SAMPLER:                  printf("-- Error at %d:  Invalid sampler\n", line); break;
            case CL_INVALID_BINARY:                   printf("-- Error at %d:  Invalid binary\n", line); break;
            case CL_INVALID_BUILD_OPTIONS:            printf("-- Error at %d:  Invalid build options\n", line); break;
            case CL_INVALID_PROGRAM:                  printf("-- Error at %d:  Invalid program\n", line); break;
            case CL_INVALID_PROGRAM_EXECUTABLE:       printf("-- Error at %d:  Invalid program executable\n", line); break;
            case CL_INVALID_KERNEL_NAME:              printf("-- Error at %d:  Invalid kernel name\n", line); break;
            case CL_INVALID_KERNEL_DEFINITION:        printf("-- Error at %d:  Invalid kernel definition\n", line); break;
            case CL_INVALID_KERNEL:                   printf("-- Error at %d:  Invalid kernel\n", line); break;
            case CL_INVALID_ARG_INDEX:                printf("-- Error at %d:  Invalid argument index\n", line); break;
            case CL_INVALID_ARG_VALUE:                printf("-- Error at %d:  Invalid argument value\n", line); break;
            case CL_INVALID_ARG_SIZE:                 printf("-- Error at %d:  Invalid argument size\n", line); break;
            case CL_INVALID_KERNEL_ARGS:              printf("-- Error at %d:  Invalid kernel arguments\n", line); break;
            case CL_INVALID_WORK_DIMENSION:           printf("-- Error at %d:  Invalid work dimensionsension\n", line); break;
            case CL_INVALID_WORK_GROUP_SIZE:          printf("-- Error at %d:  Invalid work group size\n", line); break;
            case CL_INVALID_WORK_ITEM_SIZE:           printf("-- Error at %d:  Invalid work item size\n", line); break;
            case CL_INVALID_GLOBAL_OFFSET:            printf("-- Error at %d:  Invalid global offset\n", line); break;
            case CL_INVALID_EVENT_WAIT_LIST:          printf("-- Error at %d:  Invalid event wait list\n", line); break;
            case CL_INVALID_EVENT:                    printf("-- Error at %d:  Invalid event\n", line); break;
            case CL_INVALID_OPERATION:                printf("-- Error at %d:  Invalid operation\n", line); break;
            case CL_INVALID_GL_OBJECT:                printf("-- Error at %d:  Invalid OpenGL object\n", line); break;
            case CL_INVALID_BUFFER_SIZE:              printf("-- Error at %d:  Invalid buffer size\n", line); break;
            case CL_INVALID_MIP_LEVEL:                printf("-- Error at %d:  Invalid mip-map level\n", line); break;
            case -1024:                               printf("-- Error at %d:  *clBLAS* Functionality is not implemented\n", line); break;
            case -1023:                               printf("-- Error at %d:  *clBLAS* Library is not initialized yet\n", line); break;
            case -1022:                               printf("-- Error at %d:  *clBLAS* Matrix A is not a valid memory object\n", line); break;
            case -1021:                               printf("-- Error at %d:  *clBLAS* Matrix B is not a valid memory object\n", line); break;
            case -1020:                               printf("-- Error at %d:  *clBLAS* Matrix C is not a valid memory object\n", line); break;
            case -1019:                               printf("-- Error at %d:  *clBLAS* Vector X is not a valid memory object\n", line); break;
            case -1018:                               printf("-- Error at %d:  *clBLAS* Vector Y is not a valid memory object\n", line); break;
            case -1017:                               printf("-- Error at %d:  *clBLAS* An input dimension (M,N,K) is invalid\n", line); break;
            case -1016:                               printf("-- Error at %d:  *clBLAS* Leading dimension A must not be less than the size of the first dimension\n", line); break;
            case -1015:                               printf("-- Error at %d:  *clBLAS* Leading dimension B must not be less than the size of the second dimension\n", line); break;
            case -1014:                               printf("-- Error at %d:  *clBLAS* Leading dimension C must not be less than the size of the third dimension\n", line); break;
            case -1013:                               printf("-- Error at %d:  *clBLAS* The increment for a vector X must not be 0\n", line); break;
            case -1012:                               printf("-- Error at %d:  *clBLAS* The increment for a vector Y must not be 0\n", line); break;
            case -1011:                               printf("-- Error at %d:  *clBLAS* The memory object for Matrix A is too small\n", line); break;
            case -1010:                               printf("-- Error at %d:  *clBLAS* The memory object for Matrix B is too small\n", line); break;
            case -1009:                               printf("-- Error at %d:  *clBLAS* The memory object for Matrix C is too small\n", line); break;
            case -1008:                               printf("-- Error at %d:  *clBLAS* The memory object for Vector X is too small\n", line); break;
            case -1007:                               printf("-- Error at %d:  *clBLAS* The memory object for Vector Y is too small\n", line); break;
            case -1001:                               printf("-- Error at %d:  Code -1001: no GPU available?\n", line); break;
            default:                                  printf("-- Error at %d:  Unknown with code %d\n", line, error);
        }
        exit(1);
    }
}

void printMatrix(int row, int col, const float* m) {
  std::cout << std::endl;
  for(int i=0; i<row; i++) {
    for(int j=0; j<col; j++) {
      std::cout << m[i*col + j] << " ";
    }
    std::cout << std::endl;
  }
}