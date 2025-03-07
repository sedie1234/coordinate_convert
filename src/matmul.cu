#include "matmul.cuh"

#include <cuda_runtime.h>
#include "device_launch_parameters.h"

#include <chrono>
#include <cmath>
#include <chrono>
#include <stdio.h>
#include <stdlib.h>

#define BLOCK_ROW       4
#define BLOCK_COL       8
//gridDim = (n)
//blockDim = (4x8)
//A = convert matrix
//dB = data to convert
//dC = converted data
__global__ void matMul44(float* A, float* dB, float* dC, int dataNum){

    __shared__ float dA[16];

    // memory copy : global memory -> shared memory
    dA[threadIdx.y*BLOCK_ROW + threadIdx.x] = A[threadIdx.y*BLOCK_ROW + threadIdx.x];
    __syncthreads();

    int row = threadIdx.x;
    int col = blockDim.y * blockIdx.x + threadIdx.y;

    if (row >= 4 || col >= dataNum)
	{
		return;
	}

    int Aidx = row * BLOCK_ROW;
    int Bidx = blockDim.x * blockDim.y * blockIdx.x + threadIdx.y * BLOCK_ROW;
    int Cidx = col  * BLOCK_ROW + row;


    float sum = 0;
    for(int i=0; i<BLOCK_ROW; i++){
        sum += dA[Aidx + i] * dB[Bidx + i];
    }
    dC[Cidx] = sum;
    
}

void matMul44Wrapper(float* A, float* B , float* C, int dataNum){
    float *dA, *dB, *dC;
    std::chrono::system_clock::time_point alloc_start = std::chrono::system_clock::now();
    cudaMalloc(&dA, BLOCK_ROW*BLOCK_COL*sizeof(float));
    cudaMemset(dA, 0, BLOCK_ROW*BLOCK_COL*sizeof(float));
    cudaMalloc(&dB, BLOCK_ROW*dataNum*sizeof(float));
    cudaMemset(dB, 0, BLOCK_ROW*dataNum*sizeof(float));
    cudaMalloc(&dC, BLOCK_ROW*dataNum*sizeof(float));
    cudaMemset(dC, 0, BLOCK_ROW*dataNum*sizeof(float));
    std::chrono::duration<double> timeGpuMalloc = std::chrono::system_clock::now() - alloc_start;
    printf("gpu malloc elapsed : %lf(ms)\n", timeGpuMalloc * 1000);

    std::chrono::system_clock::time_point cpy_start = std::chrono::system_clock::now();
    cudaMemcpy(dA, A, BLOCK_ROW*BLOCK_COL*sizeof(float), cudaMemcpyHostToDevice);
    cudaMemcpy(dB, B, BLOCK_ROW*dataNum*sizeof(float), cudaMemcpyHostToDevice);
    std::chrono::duration<double> timeGpuCpy = std::chrono::system_clock::now() - cpy_start;
    printf("gpu copy elapsed : %lf(ms)\n", timeGpuCpy * 1000);

    dim3 blockDim(BLOCK_ROW, BLOCK_COL);
    dim3 gridDim(ceil(static_cast<float>(dataNum) / BLOCK_COL));

    printf("Grid(%d), Block(%d, %d)\n", gridDim.x, blockDim.x, blockDim.y);
    std::chrono::system_clock::time_point start = std::chrono::system_clock::now();
    matMul44 <<<gridDim, blockDim>>> (dA, dB, dC, dataNum);
    std::chrono::duration<double> timeGpuMatmul = std::chrono::system_clock::now() - start;
    printf("gpu matmul elapsed : %lf(ms)\n", timeGpuMatmul * 1000);

    cudaDeviceSynchronize();

    cudaMemcpy(C, dC, BLOCK_ROW*dataNum*sizeof(float), cudaMemcpyDeviceToHost);
    cudaFree(dA);
    cudaFree(dB);
    cudaFree(dC);

}