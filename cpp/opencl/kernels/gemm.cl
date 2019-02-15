__kernel void gemm1(int M, int N, int K, const __global float* A, const __global float* B, __global float* C) {
  const int row = get_global_id(0); // 0 .. M-1: row index of C
  const int col = get_global_id(1); // 0 .. N-1: column index of C
  float acc = 0.0f;
  for(int i=0; i<K; i++) {
    acc += A[row * K + i] * B[i * N + col];
  }
  C[row * N + col] = acc;
}