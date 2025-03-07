#include <cuda_runtime.h>

// 벡터 덧셈을 수행하는 CUDA 커널 함수
__global__ void vectorAdd(const float* A, const float* B, float* C, int N) {
    int i = blockIdx.x * blockDim.x + threadIdx.x;
    if (i < N) {
        C[i] = A[i] + B[i];
    }
}
