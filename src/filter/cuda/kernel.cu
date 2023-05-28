#include "cuda_runtime.h"
#include "utils.cuh"
#include "stdio.h"

__global__ void convolutionKernel(cudaSurfaceObject_t surface, uint32* srcBuff, int imgWidth, int imgHeight, const float *convKernel, int nbhd) {
	// global thread indices for x and y
	const int ix = blockIdx.x * blockDim.x + threadIdx.x;
	const int iy = blockIdx.y * blockDim.y + threadIdx.y;

	if (ix > imgWidth || iy > imgHeight) {
		return;
	}
	uint32 result_pixel = 0;
	float4 result_convolve;
	if (nbhd > MAX_KERNEL_RADIUS) {
		result_convolve = convolve(ix, iy, convKernel, nbhd, srcBuff, imgWidth, imgHeight);
	} else {
		result_convolve = convolveShared(srcBuff, imgWidth, imgHeight, convKernel, nbhd);
	}
	result_convolve = clamp(result_convolve, 0.f, 1.f);
	result_pixel = toInt(result_convolve);
	surf2Dwrite(result_pixel, surface, ix * sizeof(uint32), iy, cudaBoundaryModeZero);
}

extern "C"
void runCudaKernel(cudaSurfaceObject_t glBuffer, void* deviceBuffer, int width, int height, void *convKernel, int nbhd) {
	dim3 threads(TILE_DIM, TILE_DIM);
	dim3 grid(divUp(width, TILE_DIM), divUp(height, TILE_DIM));

	convolutionKernel<<<grid, threads>>>((cudaSurfaceObject_t)glBuffer, (uint32*)deviceBuffer, width, height, (float*)convKernel, nbhd);
}
