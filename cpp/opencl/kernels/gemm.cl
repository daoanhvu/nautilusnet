#define TS 32

__kernel void gemm1(int M, int N, int K, const __global float* A, const __global float* B, __global float* C) {
  const int row = get_global_id(0); // 0 .. M-1: row index of C
  const int col = get_global_id(1); // 0 .. N-1: column index of C
  float acc = 0.0f;
  for(int i=0; i<K; i++) {
    acc += A[row * K + i] * B[i * N + col];
  }
  C[row * N + col] = acc;
}

__kernel void gemm2(const int M, const int N, const int K,
  const __global float* A, const __global float* B, __global float* C) {

    // Thread identifiers
    const int row = get_local_id(0);
    const int col = get_local_id(1);
    const int globalRow = TS * get_group_id(0) + row;
    const int globalCol = TS * get_group_id(1) + col;

    __local float subA[TS][TS];
    __local float subB[TS][TS];
    float acc = 0.0f;
    const int numTiles = K / TS;

    for(int t=0; t<numTiles; t++) {
      //Load tile data
      const int tileRow = TS * t + row;
      const int tileCol = TS * t + col;
      subA[col][row] = A[tileCol * M + globalRow];
      subB[col][row] = B[globalCol * K + tileRow];
      barrier(CLK_LOCAL_MEM_FENCE);
      barrier(CLK_LOCAL_MEM_FENCE);
    }
}